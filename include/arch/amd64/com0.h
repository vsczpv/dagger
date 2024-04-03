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

#ifndef KERNEL_ARCH_AMD64_COM0_H_
#define KERNEL_ARCH_AMD64_COM0_H_

#include <stdint.h>

#define COM0_PORT 0x3f8

#define PC_SERIAL_DATACHANNEL 0

extern const char* com0_name;

void com0_initialize_port(void);
void com0_write_data(uint8_t data);

#endif // KERNEL_ARCH_AMD64_COM0_H_
