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

#include <arch/amd64/cr.h>

intptr_t __read_cr3  (void)
{

	intptr_t pg_root;

	__asm__ volatile (
		"movq %%cr3, %0\n"
		: "=r" (pg_root)
		:
		:
	);

	return (intptr_t) pg_root;
}

void __write_cr3 (intptr_t cr3)
{
	__asm__ volatile (
		"movq %0, %%cr3"
		:
		: "r" (cr3)
		: "memory"
	);
}
