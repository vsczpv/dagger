#include <arch/amd64/x86port.h>

void __attribute__ ((naked)) outb(uint16_t, uint8_t)
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

uint8_t __attribute__ ((naked)) inb(uint16_t)
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
