#ifndef KERNEL_XTOSTR_H_
#define KERNEL_XTOSTR_H_

#include <stdint.h>

#define XTOSTR_ARRAY_SIZE 32

void itostr(char str[XTOSTR_ARRAY_SIZE], int number);
void xtostr(char str[XTOSTR_ARRAY_SIZE], uint32_t number);

void lxtostr(char str[XTOSTR_ARRAY_SIZE], uint64_t number);

#endif // KERNEL_XTOSTR_H_
