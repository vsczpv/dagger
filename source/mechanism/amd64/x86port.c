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

#include <arch/amd64/x86port.h>
#include <naked.h>

naked void outb(uint16_t, uint8_t)
{
	__asm__ volatile
	(
		"movq %%rdi, %%rdx\n"
		"movq %%rsi, %%rax\n"
		"out %%al, %%dx\n"
		"ret"
		:::
	);
}

naked uint8_t inb(uint16_t)
{
	__asm__ volatile
	(
		"movq %%rdi, %%rdx\n"
		"xorq %%rax, %%rax\n"
		"in %%dx, %%al\n"
		"ret"
		:::
	);
}
