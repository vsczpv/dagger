#ifndef KERNEL_RETURN_NOT_H_
#define KERNEL_RETURN_NOT_H_

#include <ktext.h>
#include <panic.h>

#define return_not { panic("return_not reached."); for (;;) {}; kprintf("### !!!EXTREME BUG!!! ### return_not in %s:%i bypassed.\n", __FILE__, __LINE__); }

#endif // KERNEL_RETURN_NOT_H_
