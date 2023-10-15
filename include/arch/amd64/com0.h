#ifndef KERNEL_ARCH_AMD64_COM0_H_
#define KERNEL_ARCH_AMD64_COM0_H_

#include <stdint.h>

#define COM0_PORT 0x3f8

#define PC_SERIAL_DATACHANNEL 0

extern const char* com0_name;

void com0_initialize_port(void);
void com0_write_data(uint8_t data);

#endif // KERNEL_ARCH_AMD64_COM0_H_
