#ifndef PPMIO_H
#define PPMIO_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

struct RGB
{
    unsigned char r, g, b;
};

std::vector<std::vector<RGB>> readPPM(const std::string &filename);

void writePPM(const std::string &filename, const std::vector<std::vector<RGB>> &image);

#endif
