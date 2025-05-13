#pragma once
#include <stdlib.h>
typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char   u8;

#define ARENA_SIZE_MULTIPLIER 2

__attribute__ ((alloc_size(1), hot, returns_nonnull)) void* memalloc(u32 size);

void memfree(void* p);

