#include <stdio.h>
#include "bmp.h"
#include "alloc.h"

/*local function declarations*/

__attribute__((hot)) static inline u32 __fsize (FILE* restrict fd);


/*global function definitions*/ 

const char* open_bmp(BMP* restrict bmp, FILE* f) {
  /* bmp header structure
    offset  size    field
    0      | 2      | sign
    10     | 4      | offset 
    18     | 4      | width 
    22     | 4      | height 
    28     | 2      | bit depth
  */
  
  bmp->size = __fsize(f);
  bmp->buff = memalloc(bmp->size);
  fread(bmp->buff, 1, bmp->size, f);

  if(!(bmp->buff[0] == 'B' && bmp->buff[1] == 'M'))
    return "wrong file format";

  if(*(u16*)(bmp->buff + 28) != 32)
    return "wrong bit depth";
  
  bmp->width  = *(u32*)(bmp->buff + 18);
  bmp->heigth = *(u32*)(bmp->buff + 22);
  u32 offset  = *(u32*)(bmp->buff + 10);
  
  bmp->pixels = (pixel*)(bmp->buff + offset);
  
  return 0;
}

const char* save_bmp(const BMP* restrict bmp, const char* path) {
  FILE* f = fopen(path, "wb");
  if(!f) return "failed to create file";
  
  u64 saved = fwrite(bmp->buff, bmp->size, 1, f);
  if(!saved) return "failed to save file";
  
  fclose(f);
  return 0;
}

__attribute__((hot)) 
inline pixel get_pixel(const BMP* restrict bmp, u32 idx) {
  return bmp->pixels[idx];
};

__attribute__((hot))
inline void set_pixel(const BMP* restrict bmp, u32 idx, pixel p) {
  bmp->pixels[idx] = p;
}

/*private function definitions*/
__attribute__((hot))
static inline u32 __fsize (FILE* restrict f) { 
  u32 fsize = 0;
  fseek(f, 0, SEEK_END);
  fsize = ftell(f);
  rewind(f);
  return fsize;
}

