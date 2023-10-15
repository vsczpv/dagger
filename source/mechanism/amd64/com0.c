#include <arch/amd64/com0.h>
#include <arch/amd64/x86port.h>

#include <kernel.h>

const char* com0_name = "S0";

void com0_initialize_port(void)
{

	kernel_main_serial_port.name       = com0_name;
	kernel_main_serial_port.write_data = com0_write_data;

	return;
}

static int com0_is_transmit_empty(void)
{
	return inb(COM0_PORT + 0x5) & 0x20;
}

void com0_write_data(uint8_t data)
{

	while (com0_is_transmit_empty() == 0);

	outb(COM0_PORT, data);

	return;
}

