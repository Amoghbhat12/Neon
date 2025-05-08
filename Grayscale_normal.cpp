#include <iostream>
#include <fstream>
#include <vector>

#include <sys/time.h>
#include<chrono>
struct RGB {
    unsigned char r, g, b;
};

int testing() {
    std::ifstream input("input.ppm", std::ios::binary);
   // std::ofstream output("output_nor.pgm", std::ios::binary); // Grayscale PGM format

 /*   if (!input || !output) {
        std::cerr << "Error opening file.\n";
        return 1;
    }
*/
    std::string format;
    int width, height, maxval;

    input >> format >> width >> height >> maxval;
    input.ignore(); // skip single whitespace character after header

    if (format != "P6") {
        std::cerr << "Only binary PPM (P6) format is supported.\n";
        return 1;
    }

    std::vector<RGB> pixels(width * height);
    input.read(reinterpret_cast<char*>(pixels.data()), pixels.size() * sizeof(RGB));

    // Write grayscale PGM header
   // output << "P5\n" << width << " " << height << "\n" << maxval << "\n";

    // Convert and write grayscale pixels
    for (const auto& pixel : pixels) {
        unsigned char gray = static_cast<unsigned char>(
            0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b
        );
     //   output.write(reinterpret_cast<char*>(&gray), 1);
    }

    //std::cout << "Grayscale image saved as output.pgm\n";
    return 0;
}



int main() {

    struct timeval start, end;

    long elapsed_microseconds;
   gettimeofday(&start, NULL);

for(int i = 0; i < 1000; i++) {

    testing();

}
    std::cout << "Grayscale image saved as output.pgm\n";
gettimeofday(&end, NULL);

elapsed_microseconds = ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000;


printf("Average execution time: %ld microseconds\n", elapsed_microseconds);
}


