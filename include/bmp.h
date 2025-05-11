#pragma once
#include <stdio.h>

typedef unsigned long  u64;
typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char   u8;
typedef int            i32;
  
/*global function declaration*/

typedef struct pixel24b {
  u8 B;
  u8 G;
  u8 R;
} pixel24b;

typedef struct pixel32b {
  u8 B;
  u8 G;
  u8 R;
  u8 A;
} pixel;

typedef struct BMP {
  pixel* pixels;
  u8*      buff;
  u32      size;
  u32     width;
  u32    heigth;
} BMP;

const char* open_bmp(BMP* restrict bmp, FILE* f);

const char* save_bmp(const BMP* restrict bmp, const char* path);

__attribute__((hot)) inline pixel get_pixel(const BMP* restrict bmp, u32 idx);

__attribute__((hot)) inline void set_pixel(const BMP* restrict bmp, u32 idx, pixel p);

