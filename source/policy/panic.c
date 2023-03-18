#include <ktext.h>
#include <stdbool.h>
#include <vt_escape_sequences.h>
#include <panic.h>

noreturn void ___panic(const char* restrict msg, const char* restrict file, int line)
{

	kprintf(VT_BOLD VT_RED "PANIC: " VT_WHITE "In file " VT_GREEN "%s" VT_WHITE ", line " VT_BLUE "%i" VT_WHITE ".\n" VT_END, file, line);
	kprintf("Reason: %s\n", msg);

	while (true)
	{
		__asm__ volatile
		(
			"cli\nhlt"
			:::
		);
	}
}
