#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include <stdnoreturn.h>

#define KiB 1024
#define MiB 1024*1024
#define GiB 1024*1024*1024

extern void* kernel_physical_base;
extern void* kernel_virtual_base;

noreturn void kernel_main(void);

#endif // KERNEL_KERNEL_H_
