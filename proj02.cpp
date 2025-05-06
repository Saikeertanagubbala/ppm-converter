#include "ppmio.h"
#include <iostream>
#include <vector>
#include <string>

void applyGrayscale(std::vector<std::vector<RGB>> &image) {
    for (int i{0}; i < image.size(); i++) {
        for (int j{0}; j < image.at(i).size(); j++) {
            RGB &pixel = image.at(i).at(j);
            int gray = (pixel.r + pixel.g + pixel.b)/3;
            pixel.r = static_cast<unsigned char>(gray); 
            pixel.g = static_cast<unsigned char>(gray);
            pixel.b = static_cast<unsigned char>(gray);
        }
    }
}

void applyInversion(std::vector<std::vector<RGB>> &image) {
    for (int i{0}; i < image.size(); i++) {
        for (int j{0}; j < image.at(i).size(); j++) {
            RGB &pixel = image.at(i).at(j);
            pixel.r = 255 - pixel.r; 
            pixel.g = 255 - pixel.g;
            pixel.b = 255 - pixel.b;
        }
    }
}

void applyContrastAdjustment(std::vector<std::vector<RGB>> &image) {
    const double contrastFactor{1.2}; 
    for (int i{0}; i < image.size(); i++) { 
        for (int j{0}; j < image.at(i).size(); j++) {
            RGB &pixel = image.at(i).at(j);
            int new_valR = (pixel.r - 128) * contrastFactor + 128; 
            int new_valG = (pixel.g - 128) * contrastFactor + 128;
            int new_valB = (pixel.b - 128) * contrastFactor + 128;

            if (new_valR < 0) new_valR = 0;
            if (new_valR > 255) new_valR = 255;
            if (new_valG < 0) new_valG = 0;
            if (new_valG > 255) new_valG = 255;
            if (new_valB < 0) new_valB = 0;
            if (new_valB > 255) new_valB = 255;
            pixel.r = static_cast<unsigned char>(new_valR);
            pixel.g = static_cast<unsigned char>(new_valG);
            pixel.b = static_cast<unsigned char>(new_valB);
        }
    }
}

void applyBlurring(std::vector<std::vector<RGB>> &image) {
        std::vector<std::vector<RGB>> ogImage = image;
        int height = ogImage.size();
        int width = ogImage.at(0).size(); 
        for (int i = 1; i < height-1; i++) { 
            for (int j = 1; j < width-1; j++) { 
                int sumR{0};
                int sumG{0}; 
                int sumB{0};
                for (int di{-1}; di <= 1; di++) {
                    for (int dj{-1}; dj <= 1; dj++) { 
                        RGB &neighbor = ogImage.at(i + di).at(j + dj);
                        sumR += neighbor.r;
                        sumG += neighbor.g;
                        sumB += neighbor.b;
                    }
                }
                
                image.at(i).at(j).r = static_cast<unsigned char>(sumR/9); 
                image.at(i).at(j).g = static_cast<unsigned char>(sumG/9);
                image.at(i).at(j).b = static_cast<unsigned char>(sumB/9);
            }
        }
    }
void applyMirroring(std::vector<std::vector<RGB>> &image) {
    for (int i{0}; i < image.size(); i++) {
        int width = image.at(i).size();
        std::vector<RGB> temporaryRow = image.at(i); 
        for (int j{0}; j < width; j++) {
            image.at(i).at(j) = temporaryRow.at(width-1- j); 
        }
    }
}

void applyCompression(std::vector<std::vector<RGB>> &image) {
    int height = image.size();
    int width = image.at(0).size();
    int newHeight = (height + 1) / 2; 
    int newWidth = (width + 1) / 2; 
    std::vector<std::vector<RGB>> compressedImage(newHeight, std::vector<RGB>(newWidth)); 
    int newI{0};
    for (int i{1}; i < height; i += 2) { 
        int newJ = 0;
        for (int j{1}; j< width; j += 2) {  
            compressedImage[newI][newJ] = image.at(i).at(j);
            ++newJ;
        }
        ++newI;
    }
    image = compressedImage;
}


int main(int argc, char *argv[]) { 
    if (argc < 3) {
        std::cerr << "Not enough arguments provided" << std::endl;
        return 1;
    }

    std::string inputFile, outputFile;
    std::vector<std::string> options;
    for (int i{1}; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') { 
            if (arg.size() == 2) { 
                options.push_back(arg);
            } else {
                std::cerr << "Warning: Ignoring invalid argument '" << arg << std::endl;
            }
        } else if (inputFile.empty()) {
            inputFile = arg;
        } else if (outputFile.empty()) {
            outputFile = arg;
        } else {
            std::cerr << "Warning: Ignoring unexpected argument '" << arg << std::endl; 
        }
    }

    if (inputFile.empty() || outputFile.empty()) { 
        std::cerr << "Error: Missing either input or output file." << std::endl;
        return 1;
    }

    try {
    std::vector<std::vector<RGB>> image = readPPM(inputFile);
    for (const std::string &option : options) { 
        if (option == "-g") {
            applyGrayscale(image);
        } else if (option == "-i") {
            applyInversion(image);
        } else if (option == "-x") {
            applyContrastAdjustment(image);
        } else if (option == "-b") {
            applyBlurring(image);
        } else if (option == "-m") {
            applyMirroring(image);
        } else if (option == "-c") {
            if (image.size() < 2 || image[0].size() < 2) { 
                std::cerr << "Warning: Image too small to compress, compression won't be applied." << std::endl;
            } else {
                applyCompression(image);
            }
        } else { 
            std::cerr << "Error: Unexpected argument '" << option << "'" << std::endl;
            return 1;
        }
    }
    writePPM(outputFile, image);
} catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
}