ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    MKDIR := mkdir -p
    RM := rm -rf
    QEMU := qemu-system-i386
else
    DETECTED_OS := $(shell uname -s)
    MKDIR := mkdir -p
    RM := rm -rf
    QEMU := qemu-system-i386
endif

ifeq ($(OS),Windows_NT)
    CROSS_PREFIX ?= C:/i686-elf-tools/bin/i686-elf-
else
    CROSS_PREFIX ?= i686-elf-
endif

CC := $(CROSS_PREFIX)gcc
AS := $(CROSS_PREFIX)as
LD := $(CROSS_PREFIX)ld
AR := $(CROSS_PREFIX)ar
OBJCOPY := $(CROSS_PREFIX)objcopy
OBJDUMP := $(CROSS_PREFIX)objdump
NASM := nasm

SRC_DIR := .
BUILD_DIR := build
ISO_DIR := iso
KERNEL_DIR := kernel
BOOT_DIR := boot
LIBC_DIR := libc
INCLUDE_DIR := include

KERNEL_BIN := $(BUILD_DIR)/blackos.bin
KERNEL_ELF := $(BUILD_DIR)/blackos.elf
ISO_FILE := $(BUILD_DIR)/blackos.iso

CFLAGS := -ffreestanding \
          -nostdlib \
          -nostdinc \
          -fno-builtin \
          -fstack-protector \
          -fno-pic \
          -m32 \
          -march=i686 \
          -Wall \
          -Wextra \
          -Werror \
          -g \
          -I$(INCLUDE_DIR) \
          -I$(KERNEL_DIR)/include \
          -I$(LIBC_DIR)/include

NASMFLAGS := -f elf32 -g -F dwarf

LDFLAGS := -T config/linker.ld \
           -nostdlib \
           -ffreestanding \
           -m32

LIBS := -lgcc

BOOT_ASM_SRC := $(wildcard $(BOOT_DIR)/*.asm)
KERNEL_C_SRC := $(wildcard $(KERNEL_DIR)/*.c) \
                $(wildcard $(KERNEL_DIR)/arch/x86/*.c) \
                $(wildcard $(KERNEL_DIR)/drivers/*/*.c) \
                $(wildcard $(KERNEL_DIR)/mm/*.c) \
                $(wildcard $(KERNEL_DIR)/sync/*.c) \
                $(wildcard $(KERNEL_DIR)/acpi/*.c) \
                $(wildcard $(KERNEL_DIR)/apic/*.c) \
                $(wildcard $(KERNEL_DIR)/syscall/*.c) \
                $(wildcard $(KERNEL_DIR)/shell/*.c) \
                $(wildcard $(KERNEL_DIR)/shell/cmds/*.c) \
                $(wildcard $(KERNEL_DIR)/net/*.c) \
                $(wildcard $(KERNEL_DIR)/fs/*.c) \
                $(wildcard $(KERNEL_DIR)/sched/*.c) \
                $(wildcard $(KERNEL_DIR)/smp/*.c) \
                $(wildcard $(KERNEL_DIR)/elf/*.c) \
                $(wildcard $(KERNEL_DIR)/proc/*.c) \
                $(wildcard $(KERNEL_DIR)/core/*.c) \
                $(wildcard $(KERNEL_DIR)/security/*.c) \
                $(wildcard $(KERNEL_DIR)/gui/*.c) \
                $(wildcard $(KERNEL_DIR)/ipc/*.c) \
                $(wildcard $(KERNEL_DIR)/kernel/*.c)
KERNEL_ASM_SRC := $(wildcard $(KERNEL_DIR)/arch/x86/*.asm) \
                  $(wildcard $(KERNEL_DIR)/sched/*.asm) \
                  $(wildcard $(KERNEL_DIR)/smp/*.asm)
LIBC_C_SRC := $(wildcard $(LIBC_DIR)/string/*.c) \
              $(wildcard $(LIBC_DIR)/stdio/*.c) \
              $(wildcard $(LIBC_DIR)/stdlib/*.c)

BOOT_OBJ := $(patsubst $(BOOT_DIR)/%.asm,$(BUILD_DIR)/boot/%.o,$(BOOT_ASM_SRC))
KERNEL_C_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SRC))
KERNEL_ASM_OBJ := $(patsubst %.asm,$(BUILD_DIR)/%.o,$(KERNEL_ASM_SRC))
LIBC_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(LIBC_C_SRC))

USER_C_SRC := $(wildcard usermode/lib/*.c) \
              $(wildcard usermode/bin/*.c) \
              $(wildcard usermode/shell/*.c)
USER_ASM_SRC := $(wildcard usermode/crt/*.asm)
USER_C_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(USER_C_SRC))
USER_ASM_OBJ := $(patsubst %.asm,$(BUILD_DIR)/%.o,$(USER_ASM_SRC))

ALL_OBJ := $(BOOT_OBJ) $(KERNEL_C_OBJ) $(KERNEL_ASM_OBJ) $(LIBC_OBJ)

.PHONY: all clean run debug iso dirs toolchain-check

all: dirs toolchain-check $(KERNEL_ELF)
	@echo ""
	@echo "  ██████╗ ██╗      █████╗  ██████╗██╗  ██╗     ██████╗ ███████╗"
	@echo "  ██╔══██╗██║     ██╔══██╗██╔════╝██║ ██╔╝    ██╔═══██╗██╔════╝"
	@echo "  ██████╔╝██║     ███████║██║     █████╔╝     ██║   ██║███████╗"
	@echo "  ██╔══██╗██║     ██╔══██║██║     ██╔═██╗     ██║   ██║╚════██║"
	@echo "  ██████╔╝███████╗██║  ██║╚██████╗██║  ██╗    ╚██████╔╝███████║"
	@echo "  ╚═════╝ ╚══════╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝     ╚═════╝ ╚══════╝"
	@echo ""
	@echo "  Build complete: $(KERNEL_ELF)"
	@echo ""

dirs:
	@mkdir -p $(BUILD_DIR)/boot
	@mkdir -p $(BUILD_DIR)/kernel/arch/x86
	@mkdir -p $(BUILD_DIR)/kernel/drivers/serial
	@mkdir -p $(BUILD_DIR)/kernel/drivers/keyboard
	@mkdir -p $(BUILD_DIR)/kernel/drivers/vga
	@mkdir -p $(BUILD_DIR)/kernel/drivers/mouse
	@mkdir -p $(BUILD_DIR)/kernel/drivers/pci
	@mkdir -p $(BUILD_DIR)/kernel/drivers/pit
	@mkdir -p $(BUILD_DIR)/kernel/drivers/rtc
	@mkdir -p $(BUILD_DIR)/kernel/drivers/speaker
	@mkdir -p $(BUILD_DIR)/kernel/drivers/ata
	@mkdir -p $(BUILD_DIR)/kernel/drivers/framebuffer
	@mkdir -p $(BUILD_DIR)/kernel/drivers/isolation
	@mkdir -p $(BUILD_DIR)/kernel/drivers/sb16
	@mkdir -p $(BUILD_DIR)/kernel/drivers/wav
	@mkdir -p $(BUILD_DIR)/kernel/drivers/net
	@mkdir -p $(BUILD_DIR)/kernel/mm
	@mkdir -p $(BUILD_DIR)/kernel/sync
	@mkdir -p $(BUILD_DIR)/kernel/acpi
	@mkdir -p $(BUILD_DIR)/kernel/apic
	@mkdir -p $(BUILD_DIR)/kernel/syscall
	@mkdir -p $(BUILD_DIR)/kernel/shell/cmds
	@mkdir -p $(BUILD_DIR)/kernel/fs
	@mkdir -p $(BUILD_DIR)/kernel/sched
	@mkdir -p $(BUILD_DIR)/kernel/smp
	@mkdir -p $(BUILD_DIR)/kernel/elf
	@mkdir -p $(BUILD_DIR)/kernel/proc
	@mkdir -p $(BUILD_DIR)/kernel/core
	@mkdir -p $(BUILD_DIR)/kernel/security
	@mkdir -p $(BUILD_DIR)/kernel/gui
	@mkdir -p $(BUILD_DIR)/kernel/ipc
	@mkdir -p $(BUILD_DIR)/kernel/kernel
	@mkdir -p $(BUILD_DIR)/libc/string
	@mkdir -p $(BUILD_DIR)/libc/stdio
	@mkdir -p $(BUILD_DIR)/libc/stdlib
	@mkdir -p $(BUILD_DIR)/usermode/crt
	@mkdir -p $(BUILD_DIR)/usermode/lib
	@mkdir -p $(BUILD_DIR)/usermode/bin
	@mkdir -p $(BUILD_DIR)/usermode/shell
	@mkdir -p $(ISO_DIR)/boot/grub

toolchain-check:
	@which $(CC) > /dev/null 2>&1 || \
		(echo "Error: Cross-compiler $(CC) not found!" && \
		 echo "Please build the cross-compiler first." && \
		 exit 1)

$(KERNEL_ELF): $(ALL_OBJ)
	@echo "LD $@"
	@$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/boot/%.o: $(BOOT_DIR)/%.asm
	@echo "NASM $<"
	@$(NASM) $(NASMFLAGS) -o $@ $<

$(BUILD_DIR)/kernel/%.o: $(KERNEL_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/kernel/%.o: $(KERNEL_DIR)/%.asm
	@echo "NASM $<"
	@$(NASM) $(NASMFLAGS) -o $@ $<

$(BUILD_DIR)/libc/%.o: $(LIBC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/usermode/%.o: usermode/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -Iusermode/include -c -o $@ $<

$(BUILD_DIR)/usermode/%.o: usermode/%.asm
	@mkdir -p $(dir $@)
	@echo "NASM $<"
	@$(NASM) $(NASMFLAGS) -o $@ $<

LIMINE_CD_BIN := $(ISO_DIR)/limine-cd.bin
LIMINE_SYS := $(ISO_DIR)/limine.sys

$(LIMINE_CD_BIN):
	@mkdir -p $(ISO_DIR)
	@curl -sLo $@ https://raw.githubusercontent.com/limine-bootloader/limine/v7.2.2-binary/limine-cd.bin

$(LIMINE_SYS):
	@mkdir -p $(ISO_DIR)
	@curl -sLo $@ https://raw.githubusercontent.com/limine-bootloader/limine/v7.2.2-binary/limine.sys

iso: all $(LIMINE_CD_BIN) $(LIMINE_SYS)
	@echo "Creating Limine ISO..."
	@mkdir -p $(ISO_DIR)/boot
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/blackos.elf
	@echo "TIMEOUT=0" > $(ISO_DIR)/limine.cfg
	@echo ":BlackOS PHANTOM" >> $(ISO_DIR)/limine.cfg
	@echo "    PROTOCOL=multiboot1" >> $(ISO_DIR)/limine.cfg
	@echo "    KERNEL_PATH=boot:///boot/blackos.elf" >> $(ISO_DIR)/limine.cfg
	@xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --grub2-implmac -G $(LIMINE_SYS) -o $(ISO_FILE) $(ISO_DIR) 2>/dev/null
	@echo "ISO created: $(ISO_FILE)"

DISK_IMAGES := $(wildcard disks/*.img)
DISK_ARGS := $(foreach img,$(DISK_IMAGES),-drive file=$(img),format=raw,if=ide)

run: all
	@echo "BlackOS PHANTOM booting..."
	qemu-system-i386 -kernel ./build/blackos.elf \
		-serial stdio \
		-m 256M \
		-smp 4 \
		-no-shutdown \
		-rtc base=localtime \
		-vga std \
		$(DISK_ARGS)

iso-run: iso
	@echo "BlackOS PHANTOM booting from ISO..."
	qemu-system-i386 -cdrom $(ISO_FILE) \
		-serial stdio \
		-m 256M \
		-smp 4 \
		-no-shutdown \
		-rtc base=localtime \
		-vga std \
		$(DISK_ARGS)

debug: all
	@echo "BlackOS debugger on port 1234..."
	qemu-system-i386 -kernel $(KERNEL_ELF) \
		-serial stdio \
		-m 256M \
		-no-shutdown \
		-vga std \
		$(DISK_ARGS) \
		-s -S

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)/boot/blackos.elf

help:
	@echo "BlackOS Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build kernel"
	@echo "  iso      - Create bootable ISO"
	@echo "  run      - Run in QEMU"
	@echo "  debug    - Run with GDB"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this help"
