#include "alloc.h"
#include <sorting.h>
#include <pthread.h>
#include <err.h>

/* local function declaration */
__attribute__((hot, always_inline)) static inline float hue (pixel p);
__attribute__((hot, always_inline)) static inline float light (pixel p);


static int hue_cmpr(const void *a, const void *b);
static int light_cmpr(const void *a, const void *b);

static int inv_hue_cmpr(const void *a, const void *b);
static int inv_light_cmpr(const void *a, const void *b);

void sort_chunk(pixel* buff, u32 width, u32 heigth, u8 dir, u8 ss, u32 lt, u32 ht);
void sort_chunk_updown(pixel* buff, u32 width, u32 heigth, u8 dir, u8 ss, u32 lt, u32 ht);

/* global function definition */
void pixel_sort(BMP* restrict img, u8 dir, u8 ss, u32 lt, u32 ht) {
  // u32 cores = sysconf(_SC_NPROCESSORS_ONLN);
  // pthread_t threads[cores];
  // int ids[cores];
  // for (int i = 0; i < cores; i++) {
  //       ids[i] = i + 1;
  //       int rc = pthread_create(
  //           &threads[i],
  //           NULL,
  //           qsort((pixel)),
  //           &ids[i]
  //       );
  if (dir == 'u' || dir == 'd')
    sort_chunk_updown(img->pixels, img->width, img->heigth, dir, ss, lt, ht);
  else
    sort_chunk(img->pixels, img->width, img->heigth, dir, ss, lt, ht);
}


/* local functions definitions */
__attribute__((hot, always_inline))
static inline float hue (pixel p) {
  float fr = p.R / 0xFF.0p0;
  float fg = p.G / 0xFF.0p0;
  float fb = p.B / 0xFF.0p0;
  
  float mx = fr, mn = fr;
  if (fg > mx) mx = fg;
  if (fb > mx) mx = fb;
  if (fg < mn) mn = fg;
  if (fb < mn) mn = fb;
  float delta = mx - mn;

  float h;
  if (delta == 0.0f) 
    h = 0.0f; 
  else if (mx == fr) 
    h = (fg - fb) / delta;
  else if (mx == fg)
    h = 2.0f + (fb - fr) / delta;
  else
    h = 4.0f + (fr - fg) / delta;
  
  if (h < 0.0f) 
    h += 6.0f;

  return h * 60;
}

__attribute__((hot, always_inline)) static inline float light (pixel p) {
  return (float)(p.R+p.R+p.B+p.G+p.G+p.G)/6;
}

static int light_cmpr (const void *a, const void *b) {
  pixel px1 = *(pixel*)a; 
  pixel px2 = *(pixel*)b; 

  int l1 = light(px1);
  int l2 = light(px2);

  if(l1 < l2) return -1;
  else if (l1 > l2) return 1;
  return 0;
}

static int hue_cmpr (const void *a, const void *b) {
  pixel px1 = *(pixel*)a; 
  pixel px2 = *(pixel*)b; 

  float h1 = hue(px1);
  float h2 = hue(px2);

  if (h1 < h2) return -1;
  else if (h1 > h2) return 1;
  return 0;
}

static int inv_hue_cmpr(const void *a, const void *b) {
  return -hue_cmpr(a, b);
}

static int inv_light_cmpr(const void *a, const void *b) {
  return -light_cmpr(a, b);
}

void sort_chunk(pixel* buff, u32 width, u32 heigth, u8 dir, u8 ss, u32 lt, u32 ht) {
  int (*cmpr_func)(const void*, const void*);
  float (*sst)(pixel p);
  
  switch(ss) {
    case 'h':
      if(dir == 'l')
        cmpr_func = hue_cmpr;
      else
        cmpr_func = inv_hue_cmpr;
      sst = hue;
      break;
    case 'l':
      if(dir == 'l')
        cmpr_func = light_cmpr;
      else
        cmpr_func = inv_light_cmpr;
      sst = light;
      break;
    default:
      err_terminate("Wrong sort style");
      
    }

    for (u32 row = 0; row < heigth; ++row) {
      pixel *rptr = buff + row * width;
      int start = -1;

      for (u32 x = 0; x < width; ++x) {
        float m = sst(rptr[x]);
        int in_interval = (m > lt && m < ht) ? 1 : 0;

        if (in_interval && start < 0)
          start = x;
        else if (!in_interval && start >= 0) {
            qsort(rptr + start, x - start, sizeof *rptr, cmpr_func);
            start = -1;
        }
      }

      if (start >= 0)
        qsort(rptr + start, width - start, sizeof *rptr, cmpr_func);
    }
}

void sort_chunk_updown(pixel* buff, u32 width, u32 heigth, u8 dir, u8 ss, u32 lt, u32 ht)  {
  int (*cmpr_func)(const void*, const void*);
  float (*sst)(pixel p);
  
  switch(ss) {
    case 'h':
      if(dir == 'u')
        cmpr_func = hue_cmpr;
      else
        cmpr_func = inv_hue_cmpr;
      sst = hue;
      break;
    case 'l':
      if(dir == 'd')
        cmpr_func = light_cmpr;
      else
        cmpr_func = inv_light_cmpr;
      sst = light;
      break;
    default:
      err_terminate("Wrong sort style");
  }

  for (u32 col = 0; col < width; ++col) {
    int start = -1;
    pixel *col_ptr = memalloc(heigth * sizeof(pixel));

    for (u32 row = 0; row < heigth; ++row) {
      float m = sst(buff[row * width + col]);
      int in = (m > lt && m < ht);

      if (in && start < 0)
        start = row;
      
      else if (!in && start >= 0) {
        int len = row - start;
        for (int k = 0; k < len; ++k)
            col_ptr[k] = buff[(start + k) * width + col];

        qsort(col_ptr, len, sizeof *col_ptr, cmpr_func);

        for (int k = 0; k < len; ++k)
            buff[(start + k) * width + col] = col_ptr[k];

        start = -1;
      }
    }

    if(start >= 0) {
      int len = heigth - start;

      for (int k = 0; k < len; ++k)
        col_ptr[k] = buff[(start + k) * width + col];

      qsort(col_ptr, len, sizeof *col_ptr, cmpr_func);

      for (int k = 0; k < len; ++k)
        buff[(start + k) * width + col] = col_ptr[k];
    } 
  }
}

