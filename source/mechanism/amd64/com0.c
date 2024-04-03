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

