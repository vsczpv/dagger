#ifndef KERNEL_PANIC_H_
#define KERNEL_PANIC_H_

#include <stdnoreturn.h>

#define panic(msg) ___panic(msg, __FILE__, __LINE__)

noreturn void ___panic(const char* restrict msg, const char* restrict file, int line);

#endif // KERNEL_PANIC_H
