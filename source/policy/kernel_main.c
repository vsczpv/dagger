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
#include <string.h>
#include <return_not.h>
#include <early_alloc.h>
#include <serial.h>
#include <memory.h>

/* Where the kernel is */
void* kernel_physical_base = NULL;
void* kernel_virtual_base  = NULL;

struct serial_interface kernel_main_serial_port = {0};

size_t kernel_size = 0;
size_t total_physical_memory = 0;
size_t available_physical_memory = 0;
size_t used_physical_memory = 0;

/*
 * kernel_main
 *
 * Self explanatory - The kernel's main function.
 *
 * Note that this is not the entry point, but mechanism/ARCH/start_kernel.c
 */
noreturn void kernel_main(void)
{

	/* Initialize the kernel's virtual region buddy allocator */
	memset(vmalloc_area_buddy_list, 0, sizeof (vmalloc_area_buddy_list));
	kprintfln("buddy: allocator ready, %i of virtual.", VMALLOC_AREA_SIZE);

	slab_subsystem_init();

	kprintfln("kernel made through early boot with %i bytes to spare.", early_alloc_get_remaining());

	char* c = kmalloc(sizeof (uint8_t) * 47);

	kprintfln("%X\n", c);

	slab_subsystem_dump_info();

	c[0] = 'r';
	c[1] = '\0';

	kprintfln("%s", c);

	kfree(c);

	slab_subsystem_dump_info();

	char* d = kmalloc(sizeof (uint8_t) * 511);

	slab_subsystem_dump_info();

	char* e = kmalloc(sizeof (uint8_t) * 512);

	slab_subsystem_dump_info();

	int www = (total_physical_memory - available_physical_memory) + used_physical_memory;

	char* f = kmalloc(sizeof (uint8_t) * 1*MiB);
	char* g = kmalloc(sizeof (uint8_t) * 1*MiB);
	char* h = kmalloc(sizeof (uint8_t) * 1*MiB);

	uint64_t magic = *(uint64_t*)((intptr_t) f & (~0xfff));

	kprintfln("aaaa a %X", magic);

	slab_subsystem_dump_info();

	int zzz = (total_physical_memory - available_physical_memory) + used_physical_memory;

	kprintfln("used %i\n", (zzz-www)/KiB);

	kfree(f);
	kfree(g);
	kfree(h);
	kfree(e);
	kfree(d);

	slab_subsystem_dump_info();

	return_not;
}
