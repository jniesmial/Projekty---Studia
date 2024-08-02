
// .\zad-omp.exe odcinki1.txt wynik.bmp 12

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <omp.h>
#include <cstdint>
#include <cmath>

using namespace std;

struct Point {
    int x, y;
};

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
    uint32_t dibHeaderSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t importantColors;
};
#pragma pack(pop)

void setPixel(vector<vector<uint8_t>>& image, int x, int y, uint8_t color) {
    if (x >= 0 && x < image[0].size() && y >= 0 && y < image.size()) {
        image[y][x] = color;
    }
}

void bresenhamLine(vector<vector<uint8_t>>& image, Point p1, Point p2, uint8_t color) {
    int dx = abs(p2.x - p1.x), dy = abs(p2.y - p1.y);
    int sx = (p1.x < p2.x) ? 1 : -1;
    int sy = (p1.y < p2.y) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        setPixel(image, p1.x, p1.y, color);
        if (p1.x == p2.x && p1.y == p2.y) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; p1.x += sx; }
        if (e2 < dx) { err += dx; p1.y += sy; }
    }
}

void writeBMP(const string& filename, const vector<vector<uint8_t>>& image) {
    int height = image.size();
    int width = image[0].size();
    int rowSize = (width + 3) & (~3);
    int imageSize = rowSize * height;

    BMPHeader header = { 0 };
    header.type = 0x4D42; // 'BM' in little-endian
    header.size = 54 + 256 * 4 + imageSize;
    header.offset = 54 + 256 * 4;
    header.dibHeaderSize = 40;
    header.width = width;
    header.height = height;
    header.planes = 1;
    header.bitCount = 8;
    header.imageSize = imageSize;
    header.colorsUsed = 256;

    ofstream out(filename, ios::binary);
    out.write((char*)&header, sizeof(header));

    // Write the color palette (grayscale)
    for (int i = 0; i < 256; ++i) {
        uint8_t color[4] = { (uint8_t)i, (uint8_t)i, (uint8_t)i, 0 };
        out.write((char*)color, 4);
    }

    // Write the pixel data
    for (int y = height - 1; y >= 0; --y) {
        out.write((char*)image[y].data(), width);
        for (int padding = 0; padding < rowSize - width; ++padding) {
            uint8_t pad = 0;
            out.write((char*)&pad, 1);
        }
    }

    out.close();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file> <num_threads>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];
    int numThreads = stoi(argv[3]);

    vector<pair<Point, Point>> segments;

    ifstream in(inputFile);
    if (!in) {
        cerr << "Error opening input file" << endl;
        return 1;
    }

    int x1, y1, x2, y2;
    while (in >> x1 >> y1 >> x2 >> y2) {
        segments.emplace_back(Point{ x1, y1 }, Point{ x2, y2 });
    }
    in.close();

    // Image size (example)
    int width = 500, height = 500;
    vector<vector<uint8_t>> image(height, vector<uint8_t>(width, 255)); // White background

    omp_set_num_threads(numThreads);

#pragma omp parallel for
    for (int i = 0; i < segments.size(); ++i) {
        bresenhamLine(image, segments[i].first, segments[i].second, 0); // Black line
    }

    writeBMP(outputFile, image);

    return 0;
}
