#ifndef KERNEL_LIMINE_SERVICES_H_
#define KERNEL_LIMINE_SERVICES_H_

#ifdef ARCH_amd64
#include <arch/amd64/limine.h>
#endif

extern volatile struct limine_terminal_request terminal_request;

#endif // KERNEL_LIMINE_SERVICES_H_
