ARCH = amd64

CC = cross/dest/bin/x86_64-elf-gcc
LD = cross/dest/bin/x86_64-elf-ld

OBJS_POLICY =    $(shell find source/policy            -name '*.c' -type f | sed 's/\.c$$/\.o/;s/source\//build\//')
OBJS_MECHANISM = $(shell find source/mechanism/$(ARCH) -name '*.c' -type f | sed 's/\.c$$/\.o/;s/source\//build\//')

HEADERS = $(shell find include -name '*.h' -type f)

DEFINES = -DARCH_$(ARCH)
CARGS =   -mno-red-zone -fno-pic -mcmodel=kernel -Wall -Wextra -pedantic -std=c2x -nostdlib -ffreestanding -fno-asynchronous-unwind-tables -isysteminclude $(DEFINES) \
	  -fno-stack-protector -fno-stack-check -fno-lto -fno-pie -mabi=sysv -mno-80387 -mno-mmx -mno-sse -mno-sse2

LIBGCC = cross/dest/lib/gcc/x86_64-elf/12.2.0/no-red-zone/libgcc.a

NAME = vmdagger

.PHONY: all run clean dirs

all: $(NAME)

dirs:
	@if [ ! -d "build" ] ; then mkdir build build/policy build/mechanism build/mechanism/$(ARCH) ; fi

$(OBJS_POLICY): build/policy/%.o: source/policy/%.c $(HEADERS)
	@echo "  CC      $@"
	@$(CC) -c $(CARGS) $< -o $@

$(OBJS_MECHANISM): build/mechanism/$(ARCH)/%.o: source/mechanism/$(ARCH)/%.c $(HEADERS)
	@echo "  CC      $@"
	@$(CC) -c $(CARGS) $< -o $@

$(NAME): $(OBJS_POLICY) $(OBJS_MECHANISM)
	@echo "  LD      $@"
	@$(LD) -s -static -T linker.ld $(OBJS_POLICY) $(OBJS_MECHANISM) $(LIBGCC) -o $@

clean:
	@rm -vf $(OBJS_POLICY) $(OBJS_MECHANISM) $(NAME) boot/image.iso boot/iso_root/$(NAME)

run: $(NAME)
	@cp $(NAME) boot/iso_root/$(NAME)

	@echo "  XORRISO boot/image.iso"
	@xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label boot/iso_root -o boot/image.iso 2>/dev/null

	@echo "  QEMU    boot/image.iso"
	@konsole -e 'qemu-system-x86_64 -m 512 -cdrom boot/image.iso -no-reboot -no-shutdown -nographic'
