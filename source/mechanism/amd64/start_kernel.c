#include <arch/amd64/limine.h>
#include <stdnoreturn.h>
#include <panic.h>
#include <stddef.h>
#include <kernel.h>

volatile struct limine_terminal_request terminal_request =
{
	.id = LIMINE_TERMINAL_REQUEST,
	.revision = 0
};

noreturn void start_kernel(void)
{

	if (terminal_request.response == NULL || terminal_request.response->terminal_count < 1) goto critical_early_boot_failure;

	kernel_main();

	panic("kernel_main returned.");

critical_early_boot_failure:

	for (;;) { __asm__ volatile ("cli\nhlt"); }

}
