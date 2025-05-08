#include <iostream>
#include <cstring>
#include <math.h>
#include <stdio.h>

#define ALLOWED_ERROR_THREE_DECIMAL_DIGITS  0.001f
#define HISTOGRAM_SIZE     256

#define METERING_WIDTH  64
#define METERING_HEIGHT 64

#define FLOAT_EQ(a, b, epsilon) (fabs((a) - (b)) < (epsilon))
#define MAX_OF_THREE(a,b,c) ((a>b) ? ((a>c)?a:c):((b>c)?b:c))
/*
typedef enum
{
Success = 0x00000000
} ErrorCode;

*/

typedef enum
{
    Success = 0x00000000,
    Error = 0x00000001 // Add this line
} ErrorCode;

enum { COLOR_COMPONENT_0 = 0 };
enum { COLOR_COMPONENT_1 = 1 };
enum { COLOR_COMPONENT_2 = 2 };
enum { COLOR_COMPONENT_3 = 3 };
enum { COLOR_COMPONENT_MAX = 4 };

enum { COLOR_COMPONENT_R = COLOR_COMPONENT_0 };
enum { COLOR_COMPONENT_G = COLOR_COMPONENT_1 };
enum { COLOR_COMPONENT_B = COLOR_COMPONENT_2 };
enum { COLOR_COMPONENT_GR = COLOR_COMPONENT_1 };
enum { COLOR_COMPONENT_GB = COLOR_COMPONENT_3 };

enum { ROI_NUM = 4};
enum { COLOR_COMPONENT_NUM = 4 };
enum { MAX_ROI_WINDOWS = 32 * 32 };

// Floating point types
typedef float              Float32; /* IEEE Single Precision (S1E8M23)         */
typedef double             Float64; /* IEEE Double Precision (S1E11M52)        */

typedef unsigned int       UInt32; /* 0 to 4294967295                         */
typedef   signed int       Int32; /* -2147483648 to 2147483647               */

typedef unsigned char Bool;
/**
 * This structure is used to define a 2-dimensional surface where the surface is
 * determined by it's height and width in pixels.
 */
typedef struct SizeRec
{
    /* width of the surface in pixels */
    Int32 width;

    /* height of the surface in pixels */
    Int32 height;
} Size;

typedef struct PointRec
{
    /** horizontal location of the point */
     Int32 x;

     /** vertical location of the point */
     Int32 y;
} Point;

// Function declarations
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
