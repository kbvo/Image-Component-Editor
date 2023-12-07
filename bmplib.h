#ifndef BMPLIB_H
#define BMPLIB_H

// Include for uint8_t
#include <cstdint>

const int SIZE = 256;
const int RGB = 3;
// Note: the read-write functions return 0 on success, 1 on error.

// USE THESE FOR PR4
//================================
uint8_t ***readRGBBMP(const char filename[], int& height,int& width);

int writeRGBBMP(const char filename[], uint8_t ***outputImage,int height,int width);


uint8_t** readGSBMP(const char filename[], int& height, int& width);
int writeGSBMP(const char filename[], uint8_t **outputImage,int height,int width);



// THESE ONLY WORK FOR 256x256
// read grayscale image from the file specified by filename, into inputImage
int readGSBMP(const char filename[], uint8_t image[][SIZE]);


// write grayscale image to the file specified by filename, from outputImage
int writeGSBMP(const char filename[], uint8_t outputImage[][SIZE]);

// read full-color image from the file specified by filename, into inputImage
int readRGBBMP(const char filename[], uint8_t inputImage[][SIZE][RGB]);

// read full-color image from the file specified by filename, into inputImage

// write full-color image to the file specified by filename, from outputImage
int writeRGBBMP(const char filename[], uint8_t outputImage[][SIZE][RGB]);





#endif
