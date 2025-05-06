# C++ Image Processing CLI Tool – Project 2 (CSE232)

## Overview
This command-line application processes `.ppm` image files by applying common image transformations such as grayscale, color inversion, contrast adjustment, mirroring, blurring, and compression. It reads pixel data into a 2D `std::vector<std::vector<RGB>>` and outputs the result to a new `.ppm` file. This project emphasizes practical C++ skills including vector manipulation, algorithm design, and file I/O.

---

## How to Compile
You must include the provided `ppmio.cpp` and `ppmio.h` with your `proj02.cpp` file when compiling. Use:

```bash
g++ proj02.cpp ppmio.cpp -o proj02
```
## How to Run

./proj02 input.ppm output.ppm [options]
input.ppm – path to the source image

output.ppm – desired output filename

## [options] – processing flags (can be combined)

### Options:
-g : Grayscale

-i : Invert colors

-x : Adjust contrast (factor 1.2)

-b : Box blur

-m : Mirror image horizontally

-c : Compress (halve dimensions)

-debug : Print intermediate pixel values and transformation steps (optional for development)

### Examples:

# Invert image colors
./proj02 input.ppm output.ppm -i

# Grayscale, then mirror, then compress
./proj02 input.ppm output.ppm -g -m -c

# Only convert to grayscale
./proj02 input.ppm output.ppm -g


Grayscale: Uses average of R, G, B.

Inversion: Subtract each RGB component from 255.

Contrast: Adjust via NewColor = (OriginalColor − 128) * 1.2 + 128.

Blur: Averages 3×3 neighborhood (excluding edges).

Compression: Skips every other row and column.

Error Handling: Prints helpful messages if arguments or files are invalid.

### Notes
- Only .ppm image format is supported.
