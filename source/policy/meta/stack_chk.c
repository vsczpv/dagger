#include <panic.h>
#include <ext_stdint.h>
#include <stdnoreturn.h>

#ifndef _STACK_CHK_GUARD
#error "_STACK_CHK_GUARD undefined, please check you toolchain (maybe openssl missing?)"
#endif

uintptr_t __stack_chk_guard = _STACK_CHK_GUARD;

noreturn void __stack_chk_fail(void)
{
	panic("stack smashing detected!");
}
