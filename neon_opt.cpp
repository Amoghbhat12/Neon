#include <arm_neon.h>
#include <iostream>
#include <cstring>
#include <math.h>
#include <stdio.h>
#include "autocontrol_utils.h"

#include <sys/time.h>

#include<chrono>



// Function to print the normalized histogram
/*void PrintHistogram(const Float32 *histogram) {
    for (UInt32 i = 0; i < HISTOGRAM_SIZE; i++) {
        printf("Bin %d: %f\n", i, histogram[i]);
    }
}
*/
//*************************************************************************************************
float bit_exp(float x) {
    union { float f; int i; } v;
    v.i = (int)(12102203 * x + 1065353216);
    return v.f;
}


// Fast bit-trick approximation of log(x)
float bit_log(float x) {
    union { float f; int i; } v = { x };
    return ((float)v.i - 1065353216) / 12102203.0f;
}

// Vectorized x^y using bit-log-exp trick
float32x4_t vector_pow(float32x4_t base, float32_t exponent) {
    float base_array[4];
    float result_array[4];

    vst1q_f32(base_array, base); // extract vector to array

    for (int i = 0; i < 4; i++) {
        float x = base_array[i];
        if (x > 0.0f) {
            float logx = bit_log(x);
            float result = bit_exp(exponent * logx);
            result_array[i] = result;
        } else {
            result_array[i] = 0.0f; // or define behavior for x <= 0
        }
    }

    return vld1q_f32(result_array); // load back to vector
}
//*************************************************************************************************
// Function to calculate normalized color histograms
ErrorCode CalcColorHistogramsFromStatsBlock(
    const Bool *ROIEnable,
    const Point *StartOffset,
    const Size *WindowSize,
    const UInt32 *NumWindows,
    const Float32 *pAverage_R,
    const Float32 *pAverage_G,
    const Float32 *pAverage_B,
    const Float32 *pAverage_GB,
    const UInt32 *pNumPixels_R,
    const UInt32 *pNumPixels_G,
    const UInt32 *pNumPixels_B,
    const UInt32 *pNumPixels_GB,
    Size singleRoiSize,
    const Float32 *weightTablePtr,
    Size weightTableSize,
    Float32 histogramCompressionPower,
    Float32 *histPtrOutput) {

    Float32 totalMass = 0.0f; // Variable to store the sum of histogram values
    UInt32 roiWidth = singleRoiSize.width;
    UInt32 wtWidth = weightTableSize.width;

    if (ROIEnable == NULL || NumWindows == NULL || pAverage_R == NULL || 
        pAverage_G == NULL || pAverage_B == NULL || weightTablePtr == NULL || 
        histPtrOutput == NULL) {
        printf("HistogramsFromStatsBlock: One or more parameters are null\n");
        return Error;
    }

    // Validate weight table dimensions
    if ((roiWidth * (ROI_NUM / 2)) != wtWidth) {
        printf("HistogramsFromStatsBlock: Invalid roi or weight table width\n");
        return Error;
    }

    // Initialize histogram bins to zero
    memset(histPtrOutput, 0, HISTOGRAM_SIZE * sizeof(*histPtrOutput));

    // Process ROIs and accumulate histogram values
    for (UInt32 i = 0; i < ROI_NUM; i++) {
        if (ROIEnable[i]) {
            for (UInt32 j = 0; j < NumWindows[i]; j += 4) { // Process in chunks of 4 (vectorized)
                UInt32 idx[4];
                for (int k = 0; k < 4; k++) {
                    UInt32 wtIndexForRoi = roiWidth * (i % 2) + roiWidth * wtWidth * (i / 2);
                    idx[k] = wtIndexForRoi + (j + k) % roiWidth + wtWidth * ((j + k) / roiWidth);
                }

                float32x4_t weights = vld1q_f32(&weightTablePtr[idx[0]]);
                float32x4_t r = vld1q_f32(&pAverage_R[i * MAX_ROI_WINDOWS + j]);
                float32x4_t g = vld1q_f32(&pAverage_G[i * MAX_ROI_WINDOWS + j]);
                float32x4_t b = vld1q_f32(&pAverage_B[i * MAX_ROI_WINDOWS + j]);


                // Compute maximum of r, g, b
                float32x4_t max_rg = vmaxq_f32(r, g);
                float32x4_t max_rgb = vmaxq_f32(max_rg, b);

                // Apply compression power
                float32x4_t max_value_float = vmulq_n_f32(
                    vector_pow(max_rgb, histogramCompressionPower),
                    (HISTOGRAM_SIZE - 1)
                );

                // Round to nearest integer and clamp
                uint32x4_t max_value_int = vcvtq_u32_f32(
                    vaddq_f32(max_value_float, vdupq_n_f32(0.5f))
                );
                uint32x4_t clamped_indices = vminq_u32(
                    max_value_int, 
                    vdupq_n_u32(HISTOGRAM_SIZE - 1)
                );

                // Update histogram bins
                for (int k = 0; k < 4; k++) {
                    UInt32 index = vgetq_lane_u32(clamped_indices, k);
                    histPtrOutput[index] += vgetq_lane_f32(weights, k);
                    totalMass += vgetq_lane_f32(weights, k);
                }
            }
        }
    }

    if (totalMass <= 0.0f) {
        printf("Histogram total mass is invalid\n");
        return Error;
    }

    // Normalize histogram bins
    const Float32 invTotalMass = 1.0f / totalMass;
    for (UInt32 n = 0; n < HISTOGRAM_SIZE; n++) {
        histPtrOutput[n] *= invTotalMass; // Normalize bin values
    }

    return Success;
}

int testing() {
    
    
    static const Size wtTableSize = { METERING_WIDTH, METERING_HEIGHT };
    static const Size singleRoiSize = { METERING_WIDTH / 2, METERING_HEIGHT / 2 };
    Float32 histogramCompressionPower = 1.0f;

    Bool ROIEnable[ROI_NUM] = {0};
    Point StartOffset[ROI_NUM] = {0};
    Size WindowSize[ROI_NUM] = {0};
    UInt32 NumWindows[ROI_NUM] = {0};

    Float32 pAverage_R[ROI_NUM * MAX_ROI_WINDOWS] = {0};
    Float32 pAverage_G[ROI_NUM * MAX_ROI_WINDOWS] = {0};
    Float32 pAverage_B[ROI_NUM * MAX_ROI_WINDOWS] = {0};
    Float32 pAverage_GB[ROI_NUM * MAX_ROI_WINDOWS] = {0};

    UInt32 pNumPixels_R[ROI_NUM * MAX_ROI_WINDOWS] = {0};
    UInt32 pNumPixels_G[ROI_NUM * MAX_ROI_WINDOWS] = {0};
    UInt32 pNumPixels_B[ROI_NUM * MAX_ROI_WINDOWS] = {0};
    UInt32 pNumPixels_GB[ROI_NUM * MAX_ROI_WINDOWS] = {0};

    for (UInt32 i = 0; i < ROI_NUM; i++) {
        ROIEnable[i] = true;
        NumWindows[i] = singleRoiSize.width * singleRoiSize.height;

        for (UInt32 j = 0; j < NumWindows[i]; j++) {
            pAverage_R[i * MAX_ROI_WINDOWS + j] = ((Float32)(j) / (Float32)(NumWindows[i]));
            pAverage_G[i * MAX_ROI_WINDOWS + j] = ((Float32)(j) / (Float32)(NumWindows[i]));
            pAverage_B[i * MAX_ROI_WINDOWS + j] = ((Float32)(j) / (Float32)(NumWindows[i]));
            pAverage_GB[i * MAX_ROI_WINDOWS + j] = ((Float32)(j) / (Float32)(NumWindows[i]));
        }
    }

    Float32 MatrixMeterWeightTable[METERING_HEIGHT * METERING_WIDTH];
    Float32 histComputed[HISTOGRAM_SIZE];

    for (UInt32 i = 0; i < METERING_HEIGHT * METERING_WIDTH; ++i) {
        MatrixMeterWeightTable[i] = 1.0f;

    }

  // Testcase 1: No compression
    histogramCompressionPower = 1.0f;
    CalcColorHistogramsFromStatsBlock(
        ROIEnable, StartOffset, WindowSize, NumWindows,
        pAverage_R, pAverage_G, pAverage_B, pAverage_GB,
        pNumPixels_R, pNumPixels_G, pNumPixels_B, pNumPixels_GB,
        singleRoiSize, MatrixMeterWeightTable, wtTableSize,
        histogramCompressionPower, histComputed);

    //printf("\nTestcase 1: No Compression\n");
    //PrintHistogram(histComputed);

     

 // Testcase 2: With compression
    histogramCompressionPower = 0.331008f;
    CalcColorHistogramsFromStatsBlock(
        ROIEnable, StartOffset, WindowSize, NumWindows,
        pAverage_R, pAverage_G, pAverage_B, pAverage_GB,

        pNumPixels_R, pNumPixels_G, pNumPixels_B, pNumPixels_GB,
        singleRoiSize, MatrixMeterWeightTable, wtTableSize,
        histogramCompressionPower, histComputed);

    //printf("\nTestcase 2: With Compression\n");
   // PrintHistogram(histComputed);
    

     
    return 0;
}
int main() {

    struct timeval start, end;

    long elapsed_microseconds;
   gettimeofday(&start, NULL);

for(int i = 0; i < 1000; i++) {

    testing();

}

gettimeofday(&end, NULL);

elapsed_microseconds = ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;


printf("Average execution time: %ld microseconds\n", elapsed_microseconds);
}

