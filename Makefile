#
# Copyright © 2024 Vinícius Schütz Piva
#
# This file is part of dagger
#
# dagger is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>
#

ARCH = amd64

CC = cross/dest/bin/x86_64-elf-gcc
LD = cross/dest/bin/x86_64-elf-ld

OBJS_POLICY :=    $(shell find source/policy            -name '*.c' -type f | sed 's/\.c$$/\.o/;s/source\//build\//')
OBJS_MECHANISM := $(shell find source/mechanism/$(ARCH) -name '*.c' -type f | sed 's/\.c$$/\.o/;s/source\//build\//')

HEADERS := $(shell find include -name '*.h' -type f)

DEFINES = -DARCH_$(ARCH)

# -D_STACK_CHK_GUARD is needed so that the stack protector doesn't let something slip by coincidence

STACK_CHK_GUARD_NONCE := 0x$(shell openssl rand -hex 8)

CARGS =   -mno-red-zone -fno-pic -mcmodel=kernel -Wall -Wextra -pedantic -std=c2x -nostdlib -ffreestanding -fno-asynchronous-unwind-tables -isysteminclude $(DEFINES) \
	  -fno-lto -fno-pie -mabi=sysv -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mgeneral-regs-only -fstack-protector-all -fno-stack-check \
	  -D_STACK_CHK_GUARD=$(STACK_CHK_GUARD_NONCE)

LIBGCC = cross/dest/lib/gcc/x86_64-elf/12.2.0/no-red-zone/libgcc.a

NAME = vmdagger

.PHONY: all run clean dirs

all: $(NAME)

dirs:
	@if [ ! -d "build" ] ; then mkdir build build/policy build/policy/meta build/mechanism build/mechanism/$(ARCH) ; fi
	@if [ ! -d "boot/iso_root" ] ; then mkdir boot/iso_root ; fi

$(OBJS_POLICY): build/policy/%.o: source/policy/%.c $(HEADERS)
	@echo "  CC      $@"
	@$(CC) -c $(CARGS) $< -o $@

$(OBJS_MECHANISM): build/mechanism/$(ARCH)/%.o: source/mechanism/$(ARCH)/%.c $(HEADERS)
	@echo "  CC      $@"
	@$(CC) -c $(CARGS) $< -o $@

$(NAME): dirs $(OBJS_POLICY) $(OBJS_MECHANISM)
	@echo "  LD      $@"
	@$(LD) -s -static -T linker.ld $(OBJS_POLICY) $(OBJS_MECHANISM) $(LIBGCC) -o $@

clean:
	@rm -vf $(OBJS_POLICY) $(OBJS_MECHANISM) $(NAME) boot/image.iso boot/iso_root/$(NAME)

run: $(NAME)

	@cp $(NAME) boot/iso_root/$(NAME)
	@cp boot/limine.cfg boot/iso_root/

	@echo "  XORRISO boot/image.iso"
	@xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label boot/iso_root -o boot/image.iso 2>/dev/null

	@echo "  QEMU    boot/image.iso"
	@konsole --noclose -e 'bash -c "qemu-system-x86_64 -d guest_errors -m 512M -cdrom boot/image.iso -no-reboot -no-shutdown -nographic 2>qemu_log.txt"' 2>/dev/null
