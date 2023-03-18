#ifndef KERNEL_XTOSTR_H_
#define KERNEL_XTOSTR_H_

#include <stdint.h>

#define XTOSTR_ARRAY_SIZE 16

void itostr(char str[XTOSTR_ARRAY_SIZE], int number);

#endif // KERNEL_XTOSTR_H_
