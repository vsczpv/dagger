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

#ifndef KERNEL_STRING_H_
#define KERNEL_STRING_H_

#include <stddef.h>

/* Standard C library */

size_t strlen (const char* str);
void*  memcpy (void* dest, const void* source, size_t n);
void*  memset (void* s, int c, size_t n);
void*  memmove(void* dest, const void* src, size_t n);
int    memcmp (const void* s1, const void* s2, size_t n);

#endif // KERNEL_STRING_H_
