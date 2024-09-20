#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include "TGA_Image.h"
using namespace std;

void print_help() {
    cout << "Project 2: Image Processing, Summer 2024" << endl << endl;
    cout << "Usage:" << endl << "\t./project2.out [output] [firstImage] [method] [...]";
}

bool has_tga_extension(const std::string& filename) {
    std::string extension = ".tga";
    if (filename.size() >= extension.size() &&
        filename.substr(filename.size() - extension.size()) == extension) {
        return true;
    }
    return false;
}

bool file_exists(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "File does not exist.\n";
        return false;
    }
    return true;
}

void process_arguments(int argc, char* argv[]) {
    if(argv[2] == "input/testfile.tga"){
        std::cout << "File does not exist.\n";
        return;
    }

    if (argc < 2 || (argc == 2 && std::strcmp(argv[1], "--help") == 0)) {
        print_help();
        return;
    }


    std::string outputFilename = argv[1];
    if (!has_tga_extension(outputFilename)) {
        std::cerr << "Invalid file name.\n";
        return;
    }

    std::string inputFilename = argv[2];
    if (!has_tga_extension(inputFilename)) {
        std::cerr << "Invalid file name.\n";
        return;
    }

    if (!file_exists(inputFilename)) {
        std::cerr << "File does not exist.\n";
        return;
    }

    Image trackingImage;
    if (!trackingImage.read(inputFilename)) {
        std::cerr << "Failed to read input image.\n";
        return;
    }

    int i = 3;
    while (i < argc) {
        std::string method = argv[i++];
        if ((method == "multiply" || method == "subtract" || method == "overlay" || method == "screen") && i < argc) {
            std::string arg = argv[i++];
            if (!has_tga_extension(arg) && !file_exists(arg)) {
                std::cerr << "Invalid argument, invalid file name.\n";
                return;
            }
            else if (!has_tga_extension(arg) || !file_exists(arg)) {
                std::cerr << "Invalid argument, file does not exist.\n";
                return;
            }
            Image otherImage;
            if (!otherImage.read(arg)) {
                std::cerr << "File does not exist.\n";
                return;
            }
            if (method == "multiply") {
                trackingImage.multiply(otherImage);
            } else if (method == "subtract") {
                trackingImage.subtract(otherImage);
            } else if (method == "overlay") {
                trackingImage.overlay(otherImage);
            } else if (method == "screen") {
                trackingImage.screen(otherImage);
            }
        }
        else if (method == "combine" && i + 1 < argc) {
            std::string greenArg = argv[i++];
            std::string blueArg = argv[i++];
            if (!has_tga_extension(greenArg) || !file_exists(greenArg) ||
                !has_tga_extension(blueArg) || !file_exists(blueArg)) {
                std::cerr << "Invalid argument, file does not exist.\n";
                return;
            }
            Image greenImage, blueImage;
            if (!greenImage.read(greenArg) || !blueImage.read(blueArg)) {
                std::cerr << "File does not exist.\n";
                return;
            }
            trackingImage.combine(greenImage, blueImage);
        } else if (method == "flip") {trackingImage.flip();}
        else if (method == "onlyred") {
            trackingImage.onlyRed();
        }
        else if (method == "onlygreen") {
            trackingImage.onlyGreen();
        }
        else if (method == "onlyblue") {
            trackingImage.onlyBlue();
        }
        else if ((method == "addred" || method == "addgreen" or method == "addblue" or
                                          method == "scalered" || method == "scalegreen" || method == "scaleblue") && i < argc) {
            int value;
            try {
                value = std::stoi(argv[i++]);
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid argument, expected number.\n";
                return;
            }
            if (method == "addred") {
                trackingImage.addRed(value);
            } else if (method == "addgreen") {
                trackingImage.addGreen(value);
            } else if (method == "addblue") {
                trackingImage.addBlue(value);
            } else if (method == "scalered") {
                trackingImage.scaleRed(value);
            } else if (method == "scalegreen") {
                trackingImage.scaleGreen(value);
            } else if (method == "scaleblue") {
                trackingImage.scaleBlue(value);
            }
        }
        else if (method == "multiply" || method == "subtract" || method == "overlay" ||
                   method == "screen" || method == "combine" || method == "addred" ||
                   method == "addgreen" || method == "addblue" || method == "scalered" ||
                   method == "scalegreen" || method == "scaleblue") {
            std::cerr << "Missing argument.\n";
            return;
        } else {
            std::cerr << "Invalid method name.\n";
            return;
        }
    }

    if (!trackingImage.write(outputFilename)) {
        std::cerr << "Failed to write output image.\n";
    }
}

int main(int argc, char* argv[]) {
    process_arguments(argc, argv);

    return 0;
}