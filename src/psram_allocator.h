#ifndef _PSRAM_ALLOCATOR_H
#define _PSRAM_ALLOCATOR_H

#include <Arduino.h>

// Force allocations to use PSRAM when available
inline void* psram_malloc(size_t size) {
  #ifdef BOARD_HAS_PSRAM
  void* ptr = ps_malloc(size);
  if (ptr) {
    Serial.printf("PSRAM alloc: %d bytes at %p\n", size, ptr);
    return ptr;
  }
  #endif
  // Fallback to regular heap if PSRAM fails or unavailable
  return malloc(size);
}

inline void* psram_calloc(size_t num, size_t size) {
  #ifdef BOARD_HAS_PSRAM
  void* ptr = ps_calloc(num, size);
  if (ptr) {
    return ptr;
  }
  #endif
  return calloc(num, size);
}

inline void psram_free(void* ptr) {
  if (ptr) {
    free(ptr); // Works for both PSRAM and regular heap
  }
}

#endif
