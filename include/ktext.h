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

#ifndef KERNEL_KTEXT_H_
#define KERNEL_KTEXT_H_

#include <stdarg.h>
#include <stddef.h>

#define KPRINTF_MAX_CHARS 384

int kvsnprintf(char str[], size_t size, const char* restrict format, va_list va);
int ksnprintf (char str[], size_t size, const char* restrict format, ...);

int kprintf  (const char* restrict format, ...);
int kvprintf (const char* restrict format, va_list va);
int kprintfln(const char* restrict format, ...);

void kprint  (const char* string);
void kprintln(const char* string);

#endif // KERNEL_KTEXT_H_
