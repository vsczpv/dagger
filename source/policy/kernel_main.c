#include <ktext.h>
#include <stdnoreturn.h>
#include <vt_escape_sequences.h>
#include <return_not.h>

noreturn void kernel_main(void)
{

	kprintln(VT_BOLD "Dagger 1.0 - Kernel Startup" VT_END);

	return_not;
}
