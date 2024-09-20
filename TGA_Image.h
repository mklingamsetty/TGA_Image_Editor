#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class Image {
public:
    struct Header {
        char idLength;
        char colorMapType;
        char dataTypeCode;
        short colorMapOrigin;
        short colorMapLength;
        char colorMapDepth;
        short xOrigin;
        short yOrigin;
        short width;
        short height;
        char bitsPerPixel;
        char imageDescriptor;
    };

    struct Pixel {
        unsigned char blue;
        unsigned char green;
        unsigned char red;
    };

    Image();
    bool read(const std::string &filename);
    bool write(const std::string &filename);
    Image& multiply(Image &other);
    Image& subtract(Image &other);
    Image& screen(Image &other);
    Image& overlay(Image &other);
    Image& combine(Image &greenImage, Image &blueImage);
    Image& flip();
    Image& onlyRed();
    Image& onlyGreen();
    Image& onlyBlue();
    Image& addRed(int value);
    Image& addGreen(int value);
    Image& addBlue(int value);
    Image& scaleRed(int value);
    Image& scaleGreen(int value);
    Image& scaleBlue(int value);
    int clamp(int value, int low, int high);

    // Getter and Setter for fileHeader
    Header getFileHeader() const;
    void setFileHeader(const Header &header);

    // Getter and Setter for pixels
    std::vector<Pixel>& getPixels();
    void setPixels(const std::vector<Pixel> &pix);

private:
    Header fileHeader;
    std::vector<Pixel> pixels;
};
