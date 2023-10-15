#include <serial.h>
#include <stddef.h>

void serial_write_string(struct serial_interface* port, const char* msg)
{

	size_t i = 0;

	while (msg[i]) port->write_data(msg[i++]);

	return;
}
