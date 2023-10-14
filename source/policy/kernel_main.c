#include <ktext.h>
#include <stdnoreturn.h>
#include <return_not.h>
#include <early_alloc.h>

/*
 * kernel_main
 *
 * Self explanatory - The kernel's main function.
 *
 * Note that this is not the entry point, but mechanism/ARCH/start_kernel.c
 */
noreturn void kernel_main(void)
{

	kprintfln("kernel made through early boot with %i bytes to spare.", early_alloc_get_remaining());

	return_not;
}
