#ifndef KERNEL_SERIAL_H_
#define KERNEL_SERIAL_H_

#include <stdint.h>

struct serial_interface
{
	const char* name;
	void (*write_data)(uint8_t data);
};

void serial_write_string(struct serial_interface* port, const char* msg);

#endif // KERNEL_SERIAL_H_
