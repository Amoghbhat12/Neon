#include "autocontrol_utils.h"

//Source to be optimized

/*
 * Calculates a max RGB histogram from statistics block data.
 * For each window in enabled ROIs:
 * - Finds the maximum of R,G,B values
 * - Applies histogram compression power
 * - Updates histogram using appropriate weight from weight table
 * - Normalizes final histogram so sum of bins equals 1.0
 *
 * Statistics block data is stored for each ROI in a row major array
 * i.e. statsBlock[roi][numWind ows], while weightTable is
 * stored in a single row majority array.
 */

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
    Float32 *histPtrOutput)
{
    Float32 totalMass = 0.0f;
    UInt32 roiWidth = singleRoiSize.width;
    UInt32 wtWidth = weightTableSize.width;
 
    if (ROIEnable==NULL || NumWindows==NULL || pAverage_R==NULL || pAverage_G==NULL || 
        pAverage_B==NULL || weightTablePtr==NULL || histPtrOutput==NULL)
    {
        printf("HistogramsFromStatsBlock: One or more parameters are null\n");
    }

    // Make sure weight table width must be double of roiWidth
    if ((roiWidth * (ROI_NUM / 2)) != wtWidth)
    {
        printf("HistogramsFromStatsBlock: Invalid roi or weight table width\n");
    }

    memset(histPtrOutput, 0, HISTOGRAM_SIZE * sizeof(*histPtrOutput));

    for (UInt32 i = 0; i < ROI_NUM; i++)
    {
        if (ROIEnable[i])
        {
            for (UInt32 j = 0; j < NumWindows[i]; j++)
            {
                // Inlined implementation of GetWeightTableIndexForStatsBlock
                UInt32 idx;
                if (roiWidth == 0)
                {
                    idx = 0;
                }
                else
                {
                    UInt32 wtIndexForRoi = roiWidth * (i % 2) + roiWidth * wtWidth * (i / 2);
                    idx = wtIndexForRoi + j % roiWidth + wtWidth * (j / roiWidth);
                }

                const Float32 weight = weightTablePtr[idx];
                const Float32 r = pAverage_R[i * MAX_ROI_WINDOWS + j];
                const Float32 g = pAverage_G[i * MAX_ROI_WINDOWS + j];
                const Float32 b = pAverage_B[i * MAX_ROI_WINDOWS + j];
                const Float32 maxValueFloat =
                    static_cast<Float32>(powf(MAX_OF_THREE(r,g,b), histogramCompressionPower) *
                        (HISTOGRAM_SIZE - 1));

                const UInt32 maxValueInt = static_cast<UInt32>(maxValueFloat + 0.5f);
                if (maxValueInt > (HISTOGRAM_SIZE - 1))
                {
                    printf("Calculated histogram index exceeds limit\n");
                }

                histPtrOutput[maxValueInt] += weight;
                totalMass += weight;
            }
        }
    }

    if (totalMass <= 0.0f)
    {
        printf("Histogram total mass is invalid\n");
    }

    // normalize the histogram so that total mass is 1.0.
    const Float32 invTotalMass = 1.0f / totalMass;

    for (UInt32 n = 0; n < HISTOGRAM_SIZE; n++)
    {
        if (histPtrOutput)
        {
            histPtrOutput[n] *= invTotalMass;
        }
    }

    return Success;
}
