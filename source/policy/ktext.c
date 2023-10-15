#include <ktext.h>
#include <limine_services.h>
#include <minmax.h>
#include <string.h>
#include <kernel.h>

void kprint(const char* string)
{

	/* Limine Terminal */

	struct limine_terminal* terminal = terminal_request.response->terminals[0];
	limine_terminal_write terminal_write = terminal_request.response->write;

	terminal_write(terminal, string, strlen(string));

	/* Serial Port */

	serial_write_string(&kernel_main_serial_port, string);

	return;
}

void kprintln(const char* string)
{

	kprint(string);
	kprint("\n");

	return;
}

int kprintf(const char* restrict format, ...)
{

	va_list va; va_start(va, format);

	int res = kvprintf(format, va);

	va_end(va);

	return res;
}

int kprintfln(const char* restrict format, ...)
{

	va_list va; va_start(va, format);

	int res = kvprintf(format, va);

	kprint("\n");

	va_end(va);

	return res;
}

int kvprintf(const char* restrict format, va_list va)
{

	char buffer[KPRINTF_MAX_CHARS];

	int res = kvsnprintf(buffer, KPRINTF_MAX_CHARS, format, va);
	    res = MAX(res, KPRINTF_MAX_CHARS);

	kprint(buffer);

	return res;
}
