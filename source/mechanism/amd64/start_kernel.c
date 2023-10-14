#include <arch/amd64/limine.h>
#include <stdnoreturn.h>
#include <vt_escape_sequences.h>
#include <panic.h>
#include <stddef.h>
#include <ktext.h>
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
 * This struct queries boot memory map information
 */
volatile struct limine_memmap_request memmap_request =
{
	.id       = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

/*
 * This struct queries limine for the higher half direct map address
 */
volatile struct limine_hhdm_request hhdm_request =
{
	.id       = LIMINE_HHDM_REQUEST,
	.revision = 0,
};

/*
 * This function takes the memmap info from limine and properly loads it into
 * the kernel.
 */
static void scan_limine_boot_memmaps(void)
{

	//TODO: Load info into kernel


	/* The kernel cannot continue booting without knowning it's own RAM */
	if (memmap_request.response == NULL) panic("no memmap request answered");

	/* Neither can the kernel continue without knowing where the HHDM is */
	if (hhdm_request.response == NULL) panic("no HHDM request answered");

	struct limine_memmap_response* memmap = memmap_request.response;
	struct limine_hhdm_response*   hhdm   = hhdm_request  .response;

	kprintfln("limine: HHDM virtual address: %X", hhdm->offset);

	kprintfln("limine: memory maps: %i", memmap->entry_count);

	for (size_t i = 0; i < memmap->entry_count; i++)
	{
		struct limine_memmap_entry* e = memmap->entries[i];

		size_t length = e->length;
		char*  unit   = "bytes";

		if      (length >= GiB) unit = "GiB", length /=  GiB;
		else if (length >= MiB) unit = "MiB", length /=  MiB;
		else if (length >= KiB) unit = "KiB", length /=  KiB;

		char* types[] = {"usable", "reserved", "acpi reclaimable", "acpi nvs", "bad memory", "bootloader reclaimable", "kernel", "framebuffer"};
		char* type    = types[e->type];

		kprintfln("%X %i %s\t%s", e->base, length, unit, type);
	}

	return;
}

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

	kprintln(VT_BOLD "Dagger 1.0 - Kernel Startup" VT_END);

	scan_limine_boot_memmaps();

	kernel_main();

	panic("kernel_main returned.");

critical_early_boot_failure:

	for (;;) { __asm__ volatile ("cli\nhlt"); }

}
