#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "bmplib.h"
#include "unistd.h"

using namespace std;

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;

//#define BMP_BIG_ENDIAN
#define BYTE_SWAP(num) (((num>>24)&0xff) | ((num<<8)&&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000))

typedef struct {
    uint8    bfType1;
    uint8    bfType2;
    uint32   bfSize;
    uint16   bfReserved1;
    uint16   bfReserved2;
    uint32   bfOffBits;
    uint32   biSize;          // size of structure, in bytes
    uint32   biWidth;         // bitmap width, in pixels
    uint32   biHeight;        // bitmap height, in pixels
    uint16   biPlanes;        // see below
    uint16   biBitCount;      // see below
    uint32   biCompression;   // see below
    uint32   biSizeImage;     // see below
    uint32   biXPelsPerMeter; // see below
    uint32   biYPelsPerMeter; // see below
    uint32   biClrUsed;       // see below
    uint32   biClrImportant;  // see below
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct {
    uint8 rgbBlue;
    uint8 rgbGreen;
    uint8 rgbRed;
} RGBTRIPLE;

void write_hdr(uint8 *hdr, int *hdr_idx, uint32 data, uint32 size);


// uint8 tempImage[SIZE][SIZE][RGB];
// uint8 tempGSImage[SIZE][SIZE];

unsigned char ***readRGBBMP(const char* filename, int& height,int& width)
{

    width = height = 0;
    uint8 type[2];
    //int headersize = 0;

    //int i,j,k;

    BITMAPFILEHEADER bfh;

    FILE *file;
    if (!(file=fopen(filename,"rb")))
    {
        cout << "Cannot open file: " << filename <<endl;
        return(0);
    }

    fread(type, sizeof(unsigned char), 0x2, file);
    if(type[0] != 'B' && type[1] != 'M') {
        cout << "Not a BMP file" << endl;
        return(0);
    }
    fread(&bfh.bfSize,sizeof(bfh.bfSize),1,file);
    fread(&bfh.bfReserved1,sizeof(bfh.bfReserved1),1,file);
    fread(&bfh.bfReserved2,sizeof(bfh.bfReserved2),1,file);
    fread(&bfh.bfOffBits,sizeof(bfh.bfOffBits),1,file);
    fread(&bfh.biSize,sizeof(bfh.biSize),1,file);
    fread(&bfh.biWidth,sizeof(bfh.biWidth),1,file);
    fread(&bfh.biHeight,sizeof(bfh.biHeight),1,file);

    fread(&bfh.biPlanes,sizeof(bfh.biPlanes),1,file);
    fread(&bfh.biBitCount,sizeof(bfh.biBitCount), 1, file);
    fread(&bfh.biCompression,sizeof(bfh.biCompression),1,file);
    fread(&bfh.biSizeImage,sizeof(bfh.biSizeImage),1,file);

    fread(&bfh.biXPelsPerMeter,sizeof(bfh.biXPelsPerMeter),1, file);
    fread(&bfh.biYPelsPerMeter,sizeof(bfh.biYPelsPerMeter),1, file);
    fread(&bfh.biClrUsed,sizeof(bfh.biClrUsed), 1, file);
    fread(&bfh.biClrImportant, sizeof(uint32),1,file);
    height = bfh.biHeight;
    width = bfh.biWidth;
    //cout << "ftell: " << ftell(file) << " " << height << " " << width << endl;


    unsigned char ***result=0;
    result = new unsigned char**[bfh.biHeight];
    for(unsigned i=0; i<bfh.biHeight; i++) {
        result[i] = new unsigned char*[bfh.biWidth];
        for(unsigned j=0; j<bfh.biWidth; j++) {
            result[i][j] = new unsigned char[RGB];
        }
    }

    for(unsigned i=0; i<bfh.biHeight; i++) {
        for(unsigned j=0; j<bfh.biWidth; j++) {
            char temp[3];
            fread( temp, sizeof(unsigned char),3,file);
            result[height-1-i][j][0] = temp[2];
            result[height-1-i][j][1] = temp[1];
            result[height-1-i][j][2] = temp[0];

        }
    }
    fclose(file);

    return result;
}


int writeRGBBMP(const char* filename, unsigned char ***outputImage,int height,int width)
{
    uint8 hdr[54];
    int hdr_idx = 0;

    // int i,j,k;

    BITMAPFILEHEADER bfh;

    // file pointer
    FILE *file;

    bfh.bfType1 = 'B';
    bfh.bfType2 = 'M';
    bfh.bfSize = height*width*3 + 54;
    bfh.bfReserved1 = 0x0;
    bfh.bfReserved2 = 0x0;
    bfh.bfOffBits = 0x36;

    bfh.biSize = 0x28;
    bfh.biWidth = width;
    bfh.biHeight = height;
    bfh.biPlanes = 1;
    bfh.biBitCount = 24;
    bfh.biCompression  = 0;
    bfh.biSizeImage = 3*height*width;
    bfh.biXPelsPerMeter = 0; //2400;
    bfh.biYPelsPerMeter = 0; //2400;
    bfh.biClrUsed = 0;
    bfh.biClrImportant = 0;

    write_hdr(hdr, &hdr_idx, bfh.bfType1, sizeof(uint8));
    write_hdr(hdr, &hdr_idx, bfh.bfType2, sizeof(uint8));
    write_hdr(hdr, &hdr_idx, bfh.bfSize, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.bfReserved1, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.bfReserved2, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.bfOffBits, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biSize, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biWidth, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biHeight, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biPlanes, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.biBitCount, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.biCompression, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biSizeImage, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biXPelsPerMeter, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biYPelsPerMeter, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biClrUsed, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biClrImportant, sizeof(uint32));


    // write result bmp file
    if (!(file=fopen(filename,"wb")))
    {
        cout << "Cannot open file: " << filename << endl;
        return(1);
    }
    fwrite(&hdr, sizeof(unsigned char), 0x36, file);

    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            fwrite( &outputImage[height-i-1][j][2],sizeof(unsigned char),
                    1,file);
            fwrite( &outputImage[height-i-1][j][1],sizeof(unsigned char),
                    1,file);
            fwrite( &outputImage[height-i-1][j][0],sizeof(unsigned char),
                    1,file);

        }
    }

    fclose(file);

    return 0;
}


// int writeRGBBMP(const char* filename, unsigned char ***outputImage,int height,int width)
// {
//     uint8 hdr[54];
//     int hdr_idx = 0;

//     // int i,j,k;

//     BITMAPFILEHEADER bfh;

//     // file pointer
//     FILE *file;

//     bfh.bfType1 = 'B';
//     bfh.bfType2 = 'M';
//     bfh.bfSize = 0x36;
//     bfh.bfReserved1 = 0x0;
//     bfh.bfReserved2 = 0x0;
//     bfh.bfOffBits = 0x36;

//     bfh.biSize = 0x28;
//     bfh.biWidth = width;
//     bfh.biHeight = height;
//     bfh.biPlanes = 1;
//     bfh.biBitCount = 24;
//     bfh.biCompression  = 0;
//     bfh.biSizeImage = sizeof(RGBTRIPLE)*height*width;
//     bfh.biXPelsPerMeter = 2400;
//     bfh.biYPelsPerMeter = 2400;
//     bfh.biClrUsed = 0;
//     bfh.biClrImportant = 0;

//     write_hdr(hdr, &hdr_idx, bfh.bfType1, sizeof(uint8));
//     write_hdr(hdr, &hdr_idx, bfh.bfType2, sizeof(uint8));
//     write_hdr(hdr, &hdr_idx, bfh.bfSize, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.bfReserved1, sizeof(uint16));
//     write_hdr(hdr, &hdr_idx, bfh.bfReserved2, sizeof(uint16));
//     write_hdr(hdr, &hdr_idx, bfh.bfOffBits, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.biSize, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.biWidth, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.biHeight, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.biPlanes, sizeof(uint16));
//     write_hdr(hdr, &hdr_idx, bfh.biBitCount, sizeof(uint16));
//     write_hdr(hdr, &hdr_idx, bfh.biCompression, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.biSizeImage, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.biXPelsPerMeter, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.biYPelsPerMeter, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.biClrUsed, sizeof(uint32));
//     write_hdr(hdr, &hdr_idx, bfh.biClrImportant, sizeof(uint32));


//     // write result bmp file
//     if (!(file=fopen(filename,"wb")))
//     {
//         cout << "Cannot open file: " << filename << endl;
//         return(1);
//     }
//     fwrite(&hdr, sizeof(unsigned char), 0x36, file);

//     for(int i=0; i<height; i++) {
//         for(int j=0; j<width; j++) {
//             fwrite( &outputImage[height-i-1][j][2],sizeof(unsigned char),
//                     1,file);
//             fwrite( &outputImage[height-i-1][j][1],sizeof(unsigned char),
//                     1,file);
//             fwrite( &outputImage[height-i-1][j][0],sizeof(unsigned char),
//                     1,file);

//         }
//     }

//     fclose(file);

//     return 0;
// }

void write_hdr(uint8 *hdr, int *hdr_idx, uint32 data, uint32 size)
{
    if(size == 1) {
        hdr[*hdr_idx] = (uint8) data;
        (*hdr_idx) += 1;
    }
    else if(size == 2) {
        hdr[*hdr_idx] = (uint8) (data & 0x00ff);
        (*hdr_idx) += 1;
        hdr[*hdr_idx] = (uint8) ((data & 0xff00) >> 8);
        (*hdr_idx) += 1;
    }
    else if(size == 4) {
        hdr[*hdr_idx] = (uint8) (data & 0x000000ff);
        (*hdr_idx) += 1;
        hdr[*hdr_idx] = (uint8) ((data & 0x0000ff00) >> 8);
        (*hdr_idx) += 1;
        hdr[*hdr_idx] = (uint8) ((data & 0x00ff0000) >> 16);
        (*hdr_idx) += 1;
        hdr[*hdr_idx] = (uint8) ((data & 0xff000000) >> 24);
        (*hdr_idx) += 1;
    }
    else {
        printf("Illegal size in write_hdr...consult your instructor\n");
    }
}


uint8_t** readGSBMP(const char* filename, int& height, int& width)
{
    height = width = 0;
    uint8 type[2];
    //int headersize = 0;

    //int i,j;

    BITMAPFILEHEADER bfh;

    /* file pointer */
    FILE *file;

    /* read input bmp into the data matrix */
    if (!(file=fopen(filename,"rb")))
    {
        cout << "Cannot open file: " << filename <<endl;
        return NULL;
    }

    fread(type, sizeof(unsigned char), 0x2, file);
    if(type[0] != 'B' and type[1] != 'M') {
        cout << "Not a BMP file" << endl;
        return NULL;
    }
    fread(&bfh.bfSize,sizeof(bfh.bfSize),1,file);
    fread(&bfh.bfReserved1,sizeof(bfh.bfReserved1),1,file);
    fread(&bfh.bfReserved2,sizeof(bfh.bfReserved2),1,file);
    fread(&bfh.bfOffBits,sizeof(bfh.bfOffBits),1,file);
    fread(&bfh.biSize,sizeof(bfh.biSize),1,file);
    fread(&bfh.biWidth,sizeof(bfh.biWidth),1,file);
    fread(&bfh.biHeight,sizeof(bfh.biHeight),1,file);

    fread(&bfh.biPlanes,sizeof(bfh.biPlanes),1,file);
    fread(&bfh.biBitCount,sizeof(bfh.biBitCount), 1, file);
    fread(&bfh.biCompression,sizeof(bfh.biCompression),1,file);
    fread(&bfh.biSizeImage,sizeof(bfh.biSizeImage),1,file);

    fread(&bfh.biXPelsPerMeter,sizeof(bfh.biXPelsPerMeter),1, file);
    fread(&bfh.biYPelsPerMeter,sizeof(bfh.biYPelsPerMeter),1, file);
    fread(&bfh.biClrUsed,sizeof(bfh.biClrUsed), 1, file);
    fread(&bfh.biClrImportant, sizeof(uint32),1,file);

    height = bfh.biHeight;
    width = bfh.biWidth;
    //cout << "Read header h,w = " << height << "," << width << endl;
    unsigned char **result=0;
    result = new unsigned char*[bfh.biHeight];
    for(unsigned i=0; i<bfh.biHeight; i++) {
        result[i] = new unsigned char[bfh.biWidth];
    }

    //fseek(file, 10, SEEK_SET);
    //fread(&headersize, sizeof(uint32), 1, file);
    //cout << "Header size is " << bfh.bfOffBits << endl;

    fseek(file, bfh.bfOffBits, SEEK_SET);

    for(unsigned i=0; i<bfh.biHeight; i++) {
        for(unsigned j=0; j<bfh.biWidth; j++) {
            fread( &result[height-1-i][j], sizeof(unsigned char),1,file);
        }
    }
    fclose(file);

    return result;
}



int writeBinary(const char filename[], unsigned char **outputImage,
                int height,int width) {
    unsigned char **binary = new unsigned char*[height];
    for(int i=0; i<height; i++) binary[i] = new unsigned char[width];
    for(int i=0; i<height; i++)
        for(int j=0; j<width; j++)
            binary[i][j] = (outputImage[i][j] == 1? 255 : 0 );
    int r = writeGSBMP(filename,binary,height,width);
    if(binary ) {
        for(int i=0; i<height; i++) delete [] binary[i];
        delete [] binary;
    }
    return r;

}



int writeGSBMP(const char* filename, unsigned char **outputImage,int height,int width)
{
    uint8 hdr[54];
    int hdr_idx = 0;

    //int i,j;

    BITMAPFILEHEADER bfh;

    // file pointer
    FILE *file;

    // bfh.bfType1 = 'B';
    // bfh.bfType2 = 'M';
    // // 0x10436 = 2^16 + 1024 for color def + 0x36 for header
    // bfh.bfSize = 0x010436; //0x36;

    // bfh.bfReserved1 = 0x0;
    // bfh.bfReserved2 = 0x0;
    // // 0x0436
    // bfh.bfOffBits = 0x436; //0x36;

    bfh.bfType1 = 'B';
    bfh.bfType2 = 'M';
    bfh.bfSize = width*height+0x436;
    bfh.bfReserved1 = 0x0;
    bfh.bfReserved2 = 0x0;
    bfh.bfOffBits = 0x436;

    bfh.biSize = 0x28;
    bfh.biWidth = width;
    bfh.biHeight = height;
    bfh.biPlanes = 1;
    bfh.biBitCount = 8;
    bfh.biCompression  = 0;
    bfh.biSizeImage = width*height;
    bfh.biXPelsPerMeter = 0; //2400;
    bfh.biYPelsPerMeter = 0; //2400;
    bfh.biClrUsed = 0;//width; // 0;
    bfh.biClrImportant = 0;//width; // 0;

    write_hdr(hdr, &hdr_idx, bfh.bfType1, sizeof(uint8));
    write_hdr(hdr, &hdr_idx, bfh.bfType2, sizeof(uint8));
    write_hdr(hdr, &hdr_idx, bfh.bfSize, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.bfReserved1, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.bfReserved2, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.bfOffBits, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biSize, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biWidth, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biHeight, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biPlanes, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.biBitCount, sizeof(uint16));
    write_hdr(hdr, &hdr_idx, bfh.biCompression, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biSizeImage, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biXPelsPerMeter, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biYPelsPerMeter, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biClrUsed, sizeof(uint32));
    write_hdr(hdr, &hdr_idx, bfh.biClrImportant, sizeof(uint32));


    // write result bmp file
    if (!(file=fopen(filename,"wb")))
    {
        cout << "Cannot open file: " << filename << endl;
        return(1);
    }
    //fwrite(&hdr, sizeof(unsigned char), 0x36, file);
    //
    uint8 z = 0;
    fwrite(&hdr, sizeof(unsigned char), 0x36, file);
    for(int i=0; i < 256; i++) {
        uint8 x = (uint8) i;
        //cout << (int)x << endl;
        fwrite(&x, sizeof(uint8), 0x01, file);
        fwrite(&x, sizeof(uint8), 0x01, file);
        fwrite(&x, sizeof(uint8), 0x01, file);
        fwrite(&z, sizeof(uint8), 0x01, file);

    }
    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            fwrite(&outputImage[height-1-i][j],sizeof(unsigned char),1,file);
        }
    }
    fclose(file);
    return 0;
}


