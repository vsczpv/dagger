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

#include <stdarg.h>
#include <ktext.h>
#include <stdbool.h>
#include <xtostr.h>
#include <string.h>

#define CHECK_END_OF_STR() if (str_index == size-1) \
{                                                   \
	simulated = true;                               \
	str[str_index] = '\0';                          \
}

int ksnprintf(char str[], size_t size, const char* restrict format, ...)
{

	va_list va; va_start(va, format);

	int res = kvsnprintf(str, size, format, va);

	va_end(va);

	return res;
}

int kvsnprintf(char str[], size_t size, const char* restrict format, va_list va)
{

	size_t str_index = 0;
	size_t format_len = strlen(format);

	bool in_state_machine = false;
	bool simulated = false;

	for (size_t i = 0; i < format_len; i++)
	{
		if (format[i] == '%')
		{
			in_state_machine = true;
		}
		else if (in_state_machine)
		{
			switch (format[i])
			{
				case 'i':
				{
					char result[XTOSTR_ARRAY_SIZE];
					itostr(result, va_arg(va, int));

					int j = 0;

					while (result[j])
					{
						CHECK_END_OF_STR();

						if (!simulated)
						{
							str[str_index] = result[j];
							str[str_index+1] = '\0';
						}

						str_index++, j++;
					}

					in_state_machine = false;
					break;
				}
				case 'x':
				{
					char result[XTOSTR_ARRAY_SIZE];
					xtostr(result, va_arg(va, int));

					int j = 0;

					while (result[j])
					{
						CHECK_END_OF_STR();

						if (!simulated)
						{
							str[str_index] = result[j];
							str[str_index+1] = '\0';
						}

						str_index++, j++;
					}

					in_state_machine = false;
					break;
				}
				case 'X':
				{
					char result[XTOSTR_ARRAY_SIZE];
					lxtostr(result, va_arg(va, int));

					int j = 0;

					while (result[j])
					{
						CHECK_END_OF_STR();

						if (!simulated)
						{
							str[str_index] = result[j];
							str[str_index+1] = '\0';
						}

						str_index++, j++;
					}

					in_state_machine = false;
					break;
				}
				case 's':
				{
					char* newstr   = va_arg(va, char*);

					int j = 0;

					while (newstr[j])
					{
						CHECK_END_OF_STR();

						if (!simulated)
						{
							str[str_index]   = newstr[j];
							str[str_index+1] = '\0';
						}

						str_index++, j++;
					}

					in_state_machine = false;
					break;
				}
				default:
				{
					in_state_machine = false;
					break;
				}
			}
		}
		else
		{
			CHECK_END_OF_STR();

			if (!simulated)
			{
				str[str_index]   = format[i];
				str[str_index+1] = '\0';
			}

			str_index++;
		}
	}

	return str_index;
}
