#include "autocontrol_utils.h"

#include <sys/time.h>
//Function to be tested
#include <stdio.h>
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
    Float32 *histPtrOutput);

// Function to print histogram values
/*void PrintHistogram(const Float32 *histogram) {
    for (UInt32 i = 0; i < HISTOGRAM_SIZE; i++) {
        printf("Bin %u: %f\n", i, histogram[i]);
    }
}*/
Float32 histReference_no_compression[HISTOGRAM_SIZE] = {
    0.002930 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.004883 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.004883 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.004883 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,
    0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.003906 ,0.001953
    };
    
    // Initialize a reference histogram array with predefined values (with compression)
    Float32 histReference_with_compression[HISTOGRAM_SIZE] = {
    0.000977,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,
    0.000000,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,
    0.000000,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,
    0.000000,  0.000000,  0.000977,  0.000000,  0.000000,  0.000000,  0.000000,  0.000000,
    0.000977,  0.000000,  0.000000,  0.000000,  0.000000,  0.000977,  0.000000,  0.000000,
    0.000000,  0.000977,  0.000000,  0.000000,  0.000977,  0.000000,  0.000000,  0.000977,
    0.000000,  0.000977,  0.000000,  0.000977,  0.000000,  0.000977,  0.000000,  0.000977,
    0.000000,  0.000977,  0.000000,  0.000977,  0.000977,  0.000000,  0.000977,  0.000977,
    0.000977,  0.000000,  0.000977,  0.000977,  0.000977,  0.000977,  0.000977,  0.000000,
    0.000977,  0.000977,  0.000977,  0.000977,  0.000977,  0.001953,  0.000977,  0.000977,
    0.000977,  0.000977,  0.000977,  0.001953,  0.000977,  0.000977,  0.001953,  0.000977,
    0.000977,  0.001953,  0.000977,  0.001953,  0.000977,  0.001953,  0.001953,  0.000977,
    0.001953,  0.001953,  0.000977,  0.001953,  0.001953,  0.001953,  0.001953,  0.001953,
    0.001953,  0.001953,  0.001953,  0.001953,  0.001953,  0.001953,  0.001953,  0.002930,
    0.001953,  0.001953,  0.002930,  0.001953,  0.002930,  0.001953,  0.002930,  0.001953,
    0.002930,  0.002930,  0.001953,  0.002930,  0.002930,  0.002930,  0.002930,  0.002930,
    0.002930,  0.002930,  0.002930,  0.002930,  0.002930,  0.002930,  0.003906,  0.002930,
    0.003906,  0.002930,  0.002930,  0.003906,  0.003906,  0.002930,  0.003906,  0.003906,
    0.003906,  0.002930,  0.003906,  0.003906,  0.003906,  0.004883,  0.003906,  0.003906,
    0.003906,  0.003906,  0.004883,  0.003906,  0.004883,  0.003906,  0.004883,  0.004883,
    0.003906,  0.004883,  0.004883,  0.004883,  0.004883,  0.004883,  0.004883,  0.004883,
    0.004883,  0.005859,  0.004883,  0.004883,  0.005859,  0.004883,  0.005859,  0.005859,
    0.004883,  0.005859,  0.005859,  0.005859,  0.005859,  0.005859,  0.005859,  0.005859,
    0.006836,  0.005859,  0.005859,  0.006836,  0.006836,  0.005859,  0.006836,  0.006836,
    0.005859,  0.006836,  0.006836,  0.006836,  0.006836,  0.007812,  0.006836,  0.006836,
    0.007812,  0.006836,  0.007812,  0.006836,  0.007812,  0.007812,  0.007812,  0.007812,
    0.007812,  0.007812,  0.007812,  0.007812,  0.008789,  0.007812,  0.008789,  0.007812,
    0.008789,  0.008789,  0.008789,  0.008789,  0.008789,  0.008789,  0.008789,  0.008789,
    0.008789,  0.009766,  0.008789,  0.009766,  0.009766,  0.008789,  0.009766,  0.009766,
    0.009766,  0.009766,  0.009766,  0.010742,  0.009766,  0.010742,  0.009766,  0.010742,
    0.010742,  0.009766,  0.010742,  0.010742,  0.010742,  0.011719,  0.010742,  0.010742,
    0.011719,  0.010742,  0.011719,  0.011719,  0.010742,  0.011719,  0.011719,  0.005859
    };
    
    
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
   // PrintHistogram(histComputed);

    // Testcase 2: With compression
    histogramCompressionPower = 0.331008f;
    CalcColorHistogramsFromStatsBlock(
        ROIEnable, StartOffset, WindowSize, NumWindows,
        pAverage_R, pAverage_G, pAverage_B, pAverage_GB,
        pNumPixels_R, pNumPixels_G, pNumPixels_B, pNumPixels_GB,
        singleRoiSize, MatrixMeterWeightTable, wtTableSize,
        histogramCompressionPower, histComputed);

   // printf("\nTestcase 2: With Compression\n");
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

