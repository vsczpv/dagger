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
#include <stdbool.h>
#include <vt_escape_sequences.h>
#include <panic.h>

/*
 * The underlying implementation of the panic() macro
 */
noreturn void ___panic(const char* restrict msg, const char* restrict file, int line)
{

	kprintf(VT_BOLD VT_RED "PANIC: " VT_WHITE "In file " VT_GREEN "%s" VT_WHITE ", line " VT_BLUE "%i" VT_WHITE ".\n" VT_END, file, line);
	kprintf("Reason: %s\n", msg);

	while (true)
	{
		__asm__ volatile
		(
			"cli\nhlt"
			:::
		);
	}
}
