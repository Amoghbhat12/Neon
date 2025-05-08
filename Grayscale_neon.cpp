#include <iostream>
#include <fstream>
#include <vector>
#include <arm_neon.h>
#include <sys/time.h>
#include<chrono>
using namespace std;

void rgb_to_gray_neon(const uint8_t *rgb, uint8_t *gray, int num_pixels) {
    uint8x8_t w_r = vdup_n_u8(77);   // 0.299 * 256
    uint8x8_t w_g = vdup_n_u8(150);  // 0.587 * 256
    uint8x8_t w_b = vdup_n_u8(29);   // 0.114 * 256

    int chunks = num_pixels / 8;
    for (int i = 0; i < chunks; ++i, rgb += 8 * 3, gray += 8) {
        uint8x8x3_t src = vld3_u8(rgb); // De-interleave RGB

        uint16x8_t temp = vmull_u8(src.val[0], w_r);           // R * 77
        temp = vmlal_u8(temp, src.val[1], w_g);                // + G * 150
        temp = vmlal_u8(temp, src.val[2], w_b);                // + B * 29

        uint8x8_t result = vshrn_n_u16(temp, 8);               // >> 8
        vst1_u8(gray, result);                                 // Store result
    }

    // Handle leftover pixels
    int remaining = num_pixels % 8;
    for (int i = 0; i < remaining; ++i) {
        uint8_t r = rgb[i * 3];
        uint8_t g = rgb[i * 3 + 1];
        uint8_t b = rgb[i * 3 + 2];
        gray[i] = (77 * r + 150 * g + 29 * b) >> 8;
    }
}

int testing() {
    std::ifstream input("input.ppm", std::ios::binary);
   // std::ofstream output("output_neo.pgm", std::ios::binary);

   /* if (!input || !output) {
        std::cerr << "Error opening input.ppm or output.pgm\n";
        return 1;
    }*/

    std::string format;
    int width, height, maxval;

    // Read PPM header
    input >> format >> width >> height >> maxval;
    input.ignore(); // skip single whitespace char after header

    if (format != "P6") {
        std::cerr << "Unsupported format: only binary P6 PPM supported.\n";
        return 1;
    }

    size_t num_pixels = width * height;
    std::vector<uint8_t> rgb_data(num_pixels * 3);
    std::vector<uint8_t> gray_data(num_pixels);

    input.read(reinterpret_cast<char*>(rgb_data.data()), rgb_data.size());
    if (!input) {
        std::cerr << "Error reading RGB data.\n";
        return 1;
    }

    // Convert to grayscale using NEON
    rgb_to_gray_neon(rgb_data.data(), gray_data.data(), num_pixels);

    // Write PGM (P5) header
   // output << "P5\n" << width << " " << height << "\n" << maxval << "\n";
   // output.write(reinterpret_cast<char*>(gray_data.data()), gray_data.size());

   // std::cout << "Conversion complete. Output saved as output.pgm\n";
    return 0;
}


int main() {

    struct timeval start, end;

    long elapsed_microseconds;
   gettimeofday(&start, NULL);

for(int i = 0; i < 1000; i++) {

    testing();

}
 std::cout << "Conversion complete. Output saved as output.pgm\n";
gettimeofday(&end, NULL);

elapsed_microseconds = ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;


printf("Average execution time: %ld microseconds\n", elapsed_microseconds);
}


