#ifndef KERNEL_STRING_H_
#define KERNEL_STRING_H_

#include <stddef.h>

/* Standard C library */

size_t strlen (const char* str);
void*  memcpy (void* dest, const void* source, size_t n);
void*  memset (void* s, int c, size_t n);
void*  memmove(void* dest, const void* src, size_t n);
int    memcmp (const void* s1, const void* s2, size_t n);

#endif // KERNEL_STRING_H_
