#include <alloc.h>
#include <err.h>

u32 __arena_free_space = 0x0;
u32 __arena_size = 0x0;
u8* __arena_end = 0x0;
u8* __arena = 0x0;

__attribute__((alloc_size(1), hot, returns_nonnull))
void* memalloc(u32 size)
{
  if (__arena_size == 0x0) {
    __arena_free_space += size * ARENA_GROW_FACTOR;
    __arena = malloc(__arena_size += size * ARENA_GROW_FACTOR);
    __arena_end = __arena;
  }

  if (__arena_free_space < size) {
    __arena = realloc(__arena, __arena_size += (size * ARENA_GROW_FACTOR)); 
    __arena_free_space += size * ARENA_GROW_FACTOR;
  }
  
  u8* ptr;
  ptr = __arena_end;
  __arena_end += size;
  __arena_free_space -= size;

  return ptr;
}

void memfree(void* p);
