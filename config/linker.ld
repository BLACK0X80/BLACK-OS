ENTRY(_start)

KERNEL_VMA = 0xC0000000;
KERNEL_LMA = 0x00100000;

SECTIONS
{
    . = KERNEL_LMA;

    .multiboot ALIGN(4K) :
    {
        *(.multiboot)
    }

    .boot ALIGN(4K) :
    {
        *(.boot)
        *(.boot.*)
    }

    . += KERNEL_VMA;

    _black_kernel_start = .;

    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA)
    {
        _black_text_start = .;
        *(.text)
        *(.text.*)
        _black_text_end = .;
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA)
    {
        _black_rodata_start = .;
        *(.rodata)
        *(.rodata.*)
        _black_rodata_end = .;
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA)
    {
        _black_data_start = .;
        *(.data)
        *(.data.*)
        _black_data_end = .;
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA)
    {
        _black_bss_start = .;
        *(COMMON)
        *(.bss)
        *(.bss.*)
        _black_bss_end = .;
    }

    . = ALIGN(4K);
    _black_kernel_end = .;
    _black_kernel_end_phys = . - KERNEL_VMA;
    _black_kernel_size = _black_kernel_end - KERNEL_VMA - KERNEL_LMA;

    /DISCARD/ :
    {
        *(.comment)
        *(.note.*)
        *(.eh_frame)
        *(.eh_frame_hdr)
    }
}
