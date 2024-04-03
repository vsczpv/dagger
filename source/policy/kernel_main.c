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
#include <stdnoreturn.h>
#include <return_not.h>
#include <early_alloc.h>
#include <serial.h>

/* Where the kernel is */
void* kernel_physical_base = NULL;
void* kernel_virtual_base  = NULL;

struct serial_interface kernel_main_serial_port = {0};

/*
 * kernel_main
 *
 * Self explanatory - The kernel's main function.
 *
 * Note that this is not the entry point, but mechanism/ARCH/start_kernel.c
 */
noreturn void kernel_main(void)
{

	kprintfln("kernel made through early boot with %i bytes to spare.", early_alloc_get_remaining());

	return_not;
}
