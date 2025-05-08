# NEON Image Processing with ARM Intrinsics

This project demonstrates a collection of basic image processing algorithms implemented using **ARM NEON intrinsics** for acceleration. It includes fundamental operations like RGB to Grayscale conversion, color inversion (negative), and RGB histograms, among others.

> ⚡ Optimized for ARM Cortex-A CPUs (jetson agx orin A-78)

## 📷 Features

- ✅ RGB to Grayscale Conversion
- ✅ RGB to Negative (Color Inversion)
- ✅ RGB Color Histogram
- ✅ Channel-wise Histogram
- ✅ Performance Comparison with Scalar Implementations 
- ✅ Batch Processing Support
- ✅ NEON-accelerated Pixel Operations

## 🛠 Technologies

- ARM NEON Intrinsics
- C / C++
- CMake (build system)

## 🚀 Getting Started

### Prerequisites

- A device with ARM NEON support
- C++ compiler (GCC or Clang)
- CMake
- OpenCV (for I/O)
## Instruction to run neon code
- g++ -O3 -march=armv8-a+simd -std=c++17 -pg -g neon_opt.cpp -o neontimetest
