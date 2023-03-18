#include <xtostr.h>
#include <stdbool.h>
#include <stdint.h>

void itostr(char str[XTOSTR_ARRAY_SIZE], int number)
{

	int r_index = 0;
	int s_index = 0;

	char reversed[XTOSTR_ARRAY_SIZE];

	if (number == 0)
	{
		str[0] =  '0';
		str[1] = '\0';
		return;
	}

	if (number < 0)
	{
		str[0] = '-';
		number = -number;
		s_index++;
	}

	while (number)
	{
		int module = number % 10;
		reversed[r_index++] = module + '0';
		number /= 10;
	}

	r_index--;
	while (r_index >= 0) str[s_index++] = reversed[r_index--];

	return;
}
