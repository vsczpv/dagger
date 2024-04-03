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

#ifndef KERNEL_RETURN_NOT_H_
#define KERNEL_RETURN_NOT_H_

#include <ktext.h>
#include <panic.h>

#define return_not { panic("return_not reached."); for (;;) {}; kprintf("### !!!EXTREME BUG!!! ### return_not in %s:%i bypassed.\n", __FILE__, __LINE__); }

#endif // KERNEL_RETURN_NOT_H_
