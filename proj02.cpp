#include "ppmio.h"
#include <iostream>
#include <vector>
#include <string>

// converting to grayscale: used this site for implementing the math properly https://www.quora.com/How-do-you-turn-an-image-into-grayscale-in-C
// Used this code for structuring/starting off the loop: https://gist.github.com/SubhiH/b34e74ffe4fd1aab046bcf62b7f12408
void applyGrayscale(std::vector<std::vector<RGB>> &image) {
    for (int i{0}; i < image.size(); i++) {
        for (int j{0}; j < image.at(i).size(); j++) {
            RGB &pixel = image.at(i).at(j);
            int gray = (pixel.r + pixel.g + pixel.b)/3;
            pixel.r = static_cast<unsigned char>(gray); //Used AI for this line as I wasn't aware i had to cast it afterward, helped for debugging purposes in history.txt.
            pixel.g = static_cast<unsigned char>(gray);
            pixel.b = static_cast<unsigned char>(gray);
        }
    }
}
// inverting colors: used this site to gain an understanding: https://www.geeksforgeeks.org/c-program-to-invert-making-negative-an-image-content-in-pgm-format/
void applyInversion(std::vector<std::vector<RGB>> &image) {
    for (int i{0}; i < image.size(); i++) {
        for (int j{0}; j < image.at(i).size(); j++) {
            RGB &pixel = image.at(i).at(j);
            pixel.r = 255 - pixel.r; // Used this for the actual formula of 255-value as indicated by a comment: https://cplusplus.com/forum/general/61695/
            pixel.g = 255 - pixel.g;
            pixel.b = 255 - pixel.b;
        }
    }
}
// contrast adjustment function
// Looked online for ways to do this: https://stackoverflow.com/questions/2976274/adjust-bitmap-image-brightness-contrast-using-c

void applyContrastAdjustment(std::vector<std::vector<RGB>> &image) {
    const double contrastFactor{1.2}; //Made the contrastFactor const from a piazza post that mentioned it, post 373
    for (int i{0}; i < image.size(); i++) { //These next 4 lines were copied from my previous functions as they have the same structure to just access the correct values
        for (int j{0}; j < image.at(i).size(); j++) {
            RGB &pixel = image.at(i).at(j);
            int new_valR = (pixel.r - 128) * contrastFactor + 128; //Asked chatgpt to break down the formula from project pdf and it was able to provide me a structure to use with conditionals.
            int new_valG = (pixel.g - 128) * contrastFactor + 128;
            int new_valB = (pixel.b - 128) * contrastFactor + 128;

            if (new_valR < 0) new_valR = 0; //After the formula was able to be written I had to ask for the structure on the clamping between 0 and 255, used ChatGPT to debug as i had else if statements intially and I was told it can lead to errors if multiple adjudments are in a row
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
    // Looked into this: https://stackoverflow.com/questions/41574896/ppm-image-blur-filter-creates-8-separate-images
    // this was code for a similar blurring filter that i tried to implement: https://github.com/mateochr/PPM-Image-filtering/blob/master/filter/filterblur.cpp
    // Also used piazza post 371 for clarification
        std::vector<std::vector<RGB>> ogImage = image;
        int height = ogImage.size();
        int width = ogImage.at(0).size(); //Used AI to figure out how to get width of a given image and just did .at() instead of []
        //this is for not considering the edges as specified
        for (int i = 1; i < height-1; i++) { //skips first and last row
            for (int j = 1; j < width-1; j++) { //skips first and last column
                int sumR{0};
                int sumG{0}; 
                int sumB{0};
                for (int di{-1}; di <= 1; di++) { //summing values from the neighborhood
                    for (int dj{-1}; dj <= 1; dj++) { 
                        RGB &neighbor = ogImage.at(i + di).at(j + dj);
                        sumR += neighbor.r;
                        sumG += neighbor.g;
                        sumB += neighbor.b;
                    }
                }
                //I had to use some of this for accesing values witthout worrying about undefined behavior from something like image[i][j].r so I reworked it instead to use .at().at() which ended up working: https://www.geeksforgeeks.org/nested-loops-in-c-with-examples-2/
                image.at(i).at(j).r = static_cast<unsigned char>(sumR/9); //getting average and storing back, used some of my contrast adjustment function for syntax
                image.at(i).at(j).g = static_cast<unsigned char>(sumG/9);
                image.at(i).at(j).b = static_cast<unsigned char>(sumB/9);
            }
        }
    }
//Used this for the function: https://forums.codeguru.com/showthread.php?435432-Flip-the-image-horizontally, and this youtube video for some understanding on the math https://www.youtube.com/watch?v=Yn_8KXuBXMc
void applyMirroring(std::vector<std::vector<RGB>> &image) {
    for (int i{0}; i < image.size(); i++) {
        int width = image.at(i).size();
        std::vector<RGB> temporaryRow = image.at(i); 
        for (int j{0}; j < width; j++) {
            image.at(i).at(j) = temporaryRow.at(width-1- j); 
        }
    }
}
//Looked into this: https://www.reddit.com/r/cpp_questions/comments/x07l4b/how_would_i_read_and_write_to_image_files_like/
//Used AI for structure building and consulting
void applyCompression(std::vector<std::vector<RGB>> &image) {
    int height = image.size();
    int width = image.at(0).size();
    int newHeight = (height + 1) / 2; // Used ai to make sure odd heights and widths are take care of as an edge case 
    int newWidth = (width + 1) / 2; //
    std::vector<std::vector<RGB>> compressedImage(newHeight, std::vector<RGB>(newWidth)); //"Create an empty new image to store the compressed pixels."
    int newI{0};
    for (int i{1}; i < height; i += 2) { // "For each row in the original image, if its index i is even (i%2 == 0), skip it. Otherwise, process it."
        int newJ = 0;
        for (int j{1}; j< width; j += 2) {  // "For each column in the remaining rows, if its index j is even (j%2 == 0), skip it. Otherwise,copy the pixel to the new image."
            compressedImage[newI][newJ] = image.at(i).at(j);
            ++newJ;
        }
        ++newI;
    }
    image = compressedImage;
}


int main(int argc, char *argv[]) { //https://www.geeksforgeeks.org/command-line-arguments-in-cpp/
    if (argc < 3) {
        std::cerr << "Not enough arguments provided" << std::endl;//edge case from piazza post 379
        return 1;
    }

    std::string inputFile, outputFile;
    std::vector<std::string> options;
    for (int i{1}; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') { //edge case from piazza post 379
            if (arg.size() == 2) { 
                options.push_back(arg);
            } else {
                std::cerr << "Warning: Ignoring invalid argument '" << arg << std::endl;//edge case from piazza post 379
            }
        } else if (inputFile.empty()) {
            inputFile = arg;
        } else if (outputFile.empty()) {
            outputFile = arg;
        } else {
            std::cerr << "Warning: Ignoring unexpected argument '" << arg << std::endl; //edge case from piazza post 379
        }
    }

    if (inputFile.empty() || outputFile.empty()) { //edge case from piazza post 379
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
            if (image.size() < 2 || image[0].size() < 2) { //edge case from piazza post 379
                std::cerr << "Warning: Image too small to compress, compression won't be applied." << std::endl;
            } else {
                applyCompression(image);
            }
        } else { //edge case from piazza post 379
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