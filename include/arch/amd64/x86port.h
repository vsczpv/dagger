#ifndef KERNEL_ARCH_AMD64_X86PORT_H_
#define KERNEL_ARCH_AMD64_X86PORT_H_

#include <stdint.h>

void   outb(uint16_t port_id, uint8_t data);
uint8_t inb(uint16_t port_id);

#endif // KERNEL_ARCH_AMD64_X86PORT_H_
