#include "TGA_Image.h"

Image::Image() {
    fileHeader = {0};
}

bool Image::read(const std::string &filename) {
    std::ifstream myFile(filename, std::ios::binary);
    if (!myFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    // Read each header variable individually
    myFile.read(&fileHeader.idLength, sizeof(fileHeader.idLength));
    myFile.read(&fileHeader.colorMapType, sizeof(fileHeader.colorMapType));
    myFile.read(&fileHeader.dataTypeCode, sizeof(fileHeader.dataTypeCode));
    myFile.read(reinterpret_cast<char*>(&fileHeader.colorMapOrigin), sizeof(fileHeader.colorMapOrigin));
    myFile.read(reinterpret_cast<char*>(&fileHeader.colorMapLength), sizeof(fileHeader.colorMapLength));
    myFile.read(&fileHeader.colorMapDepth, sizeof(fileHeader.colorMapDepth));
    myFile.read(reinterpret_cast<char*>(&fileHeader.xOrigin), sizeof(fileHeader.xOrigin));
    myFile.read(reinterpret_cast<char*>(&fileHeader.yOrigin), sizeof(fileHeader.yOrigin));
    myFile.read(reinterpret_cast<char*>(&fileHeader.width), sizeof(fileHeader.width));
    myFile.read(reinterpret_cast<char*>(&fileHeader.height), sizeof(fileHeader.height));
    myFile.read(&fileHeader.bitsPerPixel, sizeof(fileHeader.bitsPerPixel));
    myFile.read(&fileHeader.imageDescriptor, sizeof(fileHeader.imageDescriptor));

    if (!myFile) {
        std::cerr << "Error: Could not read header from file " << filename << std::endl;
        return false;
    }

    if (fileHeader.width <= 0 || fileHeader.height <= 0 || fileHeader.bitsPerPixel <= 0) {
        std::cerr << "Error: Invalid image dimensions or bits per pixel in file " << filename << std::endl;
        return false;
    }

    try {
        pixels.resize(fileHeader.height * fileHeader.width);
    } catch (const std::length_error &e) {
        std::cerr << "Error: " << e.what() << " in file " << filename << std::endl;
        return false;
    }

    for (auto &pixel : pixels) {
        myFile.read(reinterpret_cast<char*>(&pixel.blue), sizeof(pixel.blue));
        myFile.read(reinterpret_cast<char*>(&pixel.green), sizeof(pixel.green));
        myFile.read(reinterpret_cast<char*>(&pixel.red), sizeof(pixel.red));
    }

    if (!myFile) {
        std::cerr << "Error: Could not read pixel data from file " << filename << std::endl;
        return false;
    }

    return true;
}

bool Image::write(const std::string &filename) {
    std::ofstream myFile(filename, std::ios::binary);
    if (!myFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    // Write each header variable individually
    myFile.write(&fileHeader.idLength, sizeof(fileHeader.idLength));
    myFile.write(&fileHeader.colorMapType, sizeof(fileHeader.colorMapType));
    myFile.write(&fileHeader.dataTypeCode, sizeof(fileHeader.dataTypeCode));
    myFile.write(reinterpret_cast<const char*>(&fileHeader.colorMapOrigin), sizeof(fileHeader.colorMapOrigin));
    myFile.write(reinterpret_cast<const char*>(&fileHeader.colorMapLength), sizeof(fileHeader.colorMapLength));
    myFile.write(&fileHeader.colorMapDepth, sizeof(fileHeader.colorMapDepth));
    myFile.write(reinterpret_cast<const char*>(&fileHeader.xOrigin), sizeof(fileHeader.xOrigin));
    myFile.write(reinterpret_cast<const char*>(&fileHeader.yOrigin), sizeof(fileHeader.yOrigin));
    myFile.write(reinterpret_cast<const char*>(&fileHeader.width), sizeof(fileHeader.width));
    myFile.write(reinterpret_cast<const char*>(&fileHeader.height), sizeof(fileHeader.height));
    myFile.write(&fileHeader.bitsPerPixel, sizeof(fileHeader.bitsPerPixel));
    myFile.write(&fileHeader.imageDescriptor, sizeof(fileHeader.imageDescriptor));

    if (!myFile) {
        std::cerr << "Error: Could not write header to file " << filename << std::endl;
        return false;
    }

    for (const auto &pixel : pixels) {
        myFile.write(reinterpret_cast<const char*>(&pixel.blue), sizeof(pixel.blue));
        myFile.write(reinterpret_cast<const char*>(&pixel.green), sizeof(pixel.green));
        myFile.write(reinterpret_cast<const char*>(&pixel.red), sizeof(pixel.red));
    }

    if (!myFile) {
        std::cerr << "Error: Could not write pixel data to file " << filename << std::endl;
        return false;
    }

    return true;
}

int Image::clamp(int value, int low, int high) {
    return (value < low) ? low : (value > high) ? high : value;
}

Image& Image::multiply(Image &other) {
    for (int i = 0; i < pixels.size(); i++) {
        float p1r = pixels[i].red / 255.0f;
        float p1g = pixels[i].green / 255.0f;
        float p1b = pixels[i].blue / 255.0f;

        float p2r = other.pixels[i].red / 255.0f;
        float p2g = other.pixels[i].green / 255.0f;
        float p2b = other.pixels[i].blue / 255.0f;

        pixels[i].red = static_cast<unsigned char>(clamp((p1r * p2r) * 255 + 0.5f, 0, 255));
        pixels[i].green = static_cast<unsigned char>(clamp((p1g * p2g) * 255 + 0.5f, 0, 255));
        pixels[i].blue = static_cast<unsigned char>(clamp((p1b * p2b) * 255 + 0.5f, 0, 255));
    }
    return *this;
}

Image& Image::subtract(Image &other) {
    for (int i = 0; i < pixels.size(); i++) {
        pixels[i].red = static_cast<unsigned char>(clamp(pixels[i].red - other.pixels[i].red, 0, 255));
        pixels[i].green = static_cast<unsigned char>(clamp(pixels[i].green - other.pixels[i].green, 0, 255));
        pixels[i].blue = static_cast<unsigned char>(clamp(pixels[i].blue - other.pixels[i].blue, 0, 255));
    }
    return *this;
}

Image& Image::screen(Image &other) {
    for (int i = 0; i < pixels.size(); i++) {
        float p1r = pixels[i].red / 255.0f;
        float p1g = pixels[i].green / 255.0f;
        float p1b = pixels[i].blue / 255.0f;

        float p2r = other.pixels[i].red / 255.0f;
        float p2g = other.pixels[i].green / 255.0f;
        float p2b = other.pixels[i].blue / 255.0f;

        pixels[i].red = static_cast<unsigned char>((1 - (1 - p1r) * (1 - p2r)) * 255 + 0.5f);
        pixels[i].green = static_cast<unsigned char>((1 - (1 - p1g) * (1 - p2g)) * 255 + 0.5f);
        pixels[i].blue = static_cast<unsigned char>((1 - (1 - p1b) * (1 - p2b)) * 255 + 0.5f);
    }
    return *this;
}

Image& Image::overlay(Image &other) {
    for (int i = 0; i < pixels.size(); i++) {
        float p1r = pixels[i].red / 255.0f;
        float p1g = pixels[i].green / 255.0f;
        float p1b = pixels[i].blue / 255.0f;

        float p2r = other.pixels[i].red / 255.0f;
        float p2g = other.pixels[i].green / 255.0f;
        float p2b = other.pixels[i].blue / 255.0f;

        if (p2r <= 0.5) {
            pixels[i].red = static_cast<unsigned char>((2 * p1r * p2r) * 255 + 0.5f);
        } else {
            pixels[i].red = static_cast<unsigned char>((1 - 2 * (1 - p1r) * (1 - p2r)) * 255 + 0.5f);
        }

        if (p2g <= 0.5) {
            pixels[i].green = static_cast<unsigned char>((2 * p1g * p2g) * 255 + 0.5f);
        } else {
            pixels[i].green = static_cast<unsigned char>((1 - 2 * (1 - p1g) * (1 - p2g)) * 255 + 0.5f);
        }

        if (p2b <= 0.5) {
            pixels[i].blue = static_cast<unsigned char>((2 * p1b * p2b) * 255 + 0.5f);
        } else {
            pixels[i].blue = static_cast<unsigned char>((1 - 2 * (1 - p1b) * (1 - p2b)) * 255 + 0.5f);
        }
    }
    return *this;
}

Image& Image::combine(Image &greenImage, Image &blueImage) {
    for (int i = 0; i < pixels.size(); i++) {
        pixels[i].green = greenImage.pixels[i].green; // Green
        pixels[i].blue = blueImage.pixels[i].blue; // Blue
    }
    return *this;
}

Image& Image::flip() {
    vector<Pixel> flippedPixels;
    for (int i = pixels.size(); i > 0; i--) {
        flippedPixels.push_back(pixels[i-1]);
    }
    pixels = flippedPixels;
    return *this;
}

Image& Image::onlyRed() {
    for (auto &pixel : pixels) {
        pixel.green = pixel.red;
        pixel.blue = pixel.red;
    }
    return *this;
}

Image& Image::onlyGreen() {
    for (auto &pixel : pixels) {
        pixel.red = pixel.green;
        pixel.blue = pixel.green;
    }
    return *this;
}

Image& Image::onlyBlue() {
    for (auto &pixel : pixels) {
        pixel.red = pixel.blue;
        pixel.green = pixel.blue;
    }
    return *this;
}

Image& Image::addRed(int value) {
    for (auto &pixel : pixels) {
        pixel.red = clamp(pixel.red + value, 0, 255);
    }
    return *this;
}

Image& Image::addGreen(int value) {
    for (auto &pixel : pixels) {
        pixel.green = clamp(pixel.green + value, 0, 255);
    }
    return *this;
}

Image& Image::addBlue(int value) {
    for (auto &pixel : pixels) {
        pixel.blue = clamp(pixel.blue + value, 0, 255);
    }
    return *this;
}

Image& Image::scaleRed(int value) {
    for (auto &pixel : pixels) {
        pixel.red = clamp(pixel.red * value, 0, 255);
    }
    return *this;
}

Image& Image::scaleGreen(int value) {
    for (auto &pixel : pixels) {
        pixel.green = clamp(pixel.green * value, 0, 255);
    }
    return *this;
}

Image& Image::scaleBlue(int value) {
    for (auto &pixel : pixels) {
        pixel.blue = clamp(pixel.blue * value, 0, 255);
    }
    return *this;
}

// Getter and Setter for fileHeader
Image::Header Image::getFileHeader() const {
    return fileHeader;
}

void Image::setFileHeader(const Header &header) {
    fileHeader = header;
}

// Getter and Setter for pixels
std::vector<Image::Pixel>& Image::getPixels() {
    return pixels;
}

void Image::setPixels(const std::vector<Image::Pixel> &pix) {
    pixels = pix;
}
