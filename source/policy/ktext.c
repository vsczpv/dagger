/*
 * Copyright © 2024 Vinícius Schütz Piva
 *
 * This file is part of dagger
 *
 * dagger is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 *
 */

#include <ktext.h>
#include <limine_services.h>
#include <minmax.h>
#include <string.h>
#include <kernel.h>

void kprint(const char* string)
{

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
