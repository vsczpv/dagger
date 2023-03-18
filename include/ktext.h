#ifndef KERNEL_KTEXT_H_
#define KERNEL_KTEXT_H_

#include <stdarg.h>
#include <stddef.h>

#define KPRINTF_MAX_CHARS 384

int kvsnprintf(char str[], size_t size, const char* restrict format, va_list va);
int ksnprintf (char str[], size_t size, const char* restrict format, ...);

int kprintf  (const char* restrict format, ...);
int kvprintf (const char* restrict format, va_list va);
int kprintfln(const char* restrict format, ...);

void kprint  (const char* string);
void kprintln(const char* string);

#endif // KERNEL_KTEXT_H_
