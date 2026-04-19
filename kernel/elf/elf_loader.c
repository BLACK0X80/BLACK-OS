#include <kernel/kernel.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <string.h>

#define BLACK_ELF_MAGIC  0x464C457F
#define BLACK_ET_EXEC    2
#define BLACK_EM_386     3
#define BLACK_PT_LOAD    1

typedef struct {
    uint32_t black_magic;
    uint8_t  black_class;
    uint8_t  black_data;
    uint8_t  black_version;
    uint8_t  black_osabi;
    uint8_t  black_pad[8];
    uint16_t black_type;
    uint16_t black_machine;
    uint32_t black_version2;
    uint32_t black_entry;
    uint32_t black_phoff;
    uint32_t black_shoff;
    uint32_t black_flags;
    uint16_t black_ehsize;
    uint16_t black_phentsize;
    uint16_t black_phnum;
    uint16_t black_shentsize;
    uint16_t black_shnum;
    uint16_t black_shstrndx;
} __attribute__((packed)) black_elf_header_t;

typedef struct {
    uint32_t black_type;
    uint32_t black_offset;
    uint32_t black_vaddr;
    uint32_t black_paddr;
    uint32_t black_filesz;
    uint32_t black_memsz;
    uint32_t black_flags;
    uint32_t black_align;
} __attribute__((packed)) black_elf_phdr_t;

int black_elf_validate(void *black_data)
{
    black_elf_header_t *black_h = (black_elf_header_t *)black_data;
    if (black_h->black_magic != BLACK_ELF_MAGIC) return -1;
    if (black_h->black_class != 1) return -2;
    if (black_h->black_machine != BLACK_EM_386) return -3;
    if (black_h->black_type != BLACK_ET_EXEC) return -4;
    return 0;
}

uint32_t black_elf_load(void *black_data, uint32_t black_size)
{
    (void)black_size;
    if (black_elf_validate(black_data) != 0) return 0;

    black_elf_header_t *black_h = (black_elf_header_t *)black_data;
    black_elf_phdr_t *black_phdrs = (black_elf_phdr_t *)((uint32_t)black_data + black_h->black_phoff);

    for (int black_i = 0; black_i < black_h->black_phnum; black_i++) {
        if (black_phdrs[black_i].black_type != BLACK_PT_LOAD) continue;
        uint32_t black_vaddr = black_phdrs[black_i].black_vaddr;
        uint32_t black_memsz = black_phdrs[black_i].black_memsz;
        uint32_t black_filesz = black_phdrs[black_i].black_filesz;
        uint32_t black_offset = black_phdrs[black_i].black_offset;
        uint32_t black_flags = PAGE_PRESENT | PAGE_USER;
        if (black_phdrs[black_i].black_flags & 0x02) black_flags |= PAGE_WRITE;

        for (uint32_t black_off = 0; black_off < black_memsz; black_off += PAGE_SIZE) {
            uint32_t black_phys = black_pmm_alloc_frame();
            if (!black_phys) return 0;
            black_vmm_map_page(black_vaddr + black_off, black_phys, black_flags);
        }

        if (black_filesz > 0)
            memcpy((void *)black_vaddr, (void *)((uint32_t)black_data + black_offset), black_filesz);
        if (black_memsz > black_filesz)
            memset((void *)(black_vaddr + black_filesz), 0, black_memsz - black_filesz);
    }

    return black_h->black_entry;
}
