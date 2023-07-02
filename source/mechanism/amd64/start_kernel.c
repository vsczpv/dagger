#include <arch/amd64/limine.h>
#include <stdnoreturn.h>
#include <panic.h>
#include <stddef.h>
#include <kernel.h>

/*
 * This struct must be volatile to guarantee it is part of the object file;
 * Limine will look for it at boot time and fill in the response field.
 */
volatile struct limine_terminal_request terminal_request =
{
	.id = LIMINE_TERMINAL_REQUEST,
	.revision = 0
};

/*
 * start_kernel
 *
 * This is the amd64 entrypoint.
 *
 * After the conclusion of platform specific initialization, it jumps to kernel_main().
 */
noreturn void start_kernel(void)
{

	/* Limine should've provided a nice high-res vt console for us, so halt the system in case it has failed to do so. */
	if (terminal_request.response == NULL || terminal_request.response->terminal_count < 1) goto critical_early_boot_failure;

	kernel_main();

	panic("kernel_main returned.");

critical_early_boot_failure:

	for (;;) { __asm__ volatile ("cli\nhlt"); }

}
