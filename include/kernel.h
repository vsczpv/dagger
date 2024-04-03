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

#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include <stdnoreturn.h>
#include <serial.h>

#define KiB 1024
#define MiB 1024*1024
#define GiB 1024*1024*1024

extern void* kernel_physical_base;
extern void* kernel_virtual_base;

extern struct serial_interface kernel_main_serial_port;

noreturn void kernel_main(void);

#endif // KERNEL_KERNEL_H_
