#![no_std]
#![allow(non_camel_case_types)]

#[repr(C)]
pub struct BlackElfHeader {
    pub black_magic: [u8; 4],
    pub black_class: u8,
    pub black_data: u8,
    pub black_version: u8,
    pub black_osabi: u8,
    pub black_padding: [u8; 8],
    pub black_type: u16,
    pub black_machine: u16,
    pub black_elf_version: u32,
    pub black_entry: u32,
    pub black_phoff: u32,
    pub black_shoff: u32,
    pub black_flags: u32,
    pub black_ehsize: u16,
    pub black_phentsize: u16,
    pub black_phnum: u16,
    pub black_shentsize: u16,
    pub black_shnum: u16,
    pub black_shstrndx: u16,
}

#[repr(C)]
pub struct BlackElfProgramHeader {
    pub black_type: u32,
    pub black_offset: u32,
    pub black_vaddr: u32,
    pub black_paddr: u32,
    pub black_filesz: u32,
    pub black_memsz: u32,
    pub black_flags: u32,
    pub black_align: u32,
}

#[repr(C)]
pub struct BlackElfSectionHeader {
    pub black_name: u32,
    pub black_type: u32,
    pub black_flags: u32,
    pub black_addr: u32,
    pub black_offset: u32,
    pub black_size: u32,
    pub black_link: u32,
    pub black_info: u32,
    pub black_addralign: u32,
    pub black_entsize: u32,
}

#[repr(C)]
pub struct BlackElfInfo {
    pub black_valid: i32,
    pub black_entry: u32,
    pub black_phnum: u16,
    pub black_shnum: u16,
    pub black_type: u16,
    pub black_machine: u16,
    pub black_text_size: u32,
    pub black_data_size: u32,
    pub black_bss_size: u32,
}

const BLACK_PT_NULL: u32 = 0;
const BLACK_PT_LOAD: u32 = 1;
const BLACK_SHT_PROGBITS: u32 = 1;
const BLACK_SHT_NOBITS: u32 = 8;
const BLACK_SHF_EXECINSTR: u32 = 4;
const BLACK_SHF_WRITE: u32 = 1;

#[no_mangle]
pub extern "C" fn black_elf_verify_rust(black_data: *const u8, black_len: usize) -> i32 {
    if black_data.is_null() || black_len < 52 {
        return -1;
    }

    let black_header = unsafe {
        &*(black_data as *const BlackElfHeader)
    };

    if black_header.black_magic[0] != 0x7F
        || black_header.black_magic[1] != b'E'
        || black_header.black_magic[2] != b'L'
        || black_header.black_magic[3] != b'F'
    {
        return -2;
    }

    if black_header.black_class != 1 {
        return -3;
    }

    if black_header.black_data != 1 {
        return -4;
    }

    if black_header.black_machine != 3 {
        return -5;
    }

    if black_header.black_phoff as usize > black_len {
        return -6;
    }

    if black_header.black_shoff as usize > black_len {
        return -7;
    }

    let black_ph_end = black_header.black_phoff as usize
        + (black_header.black_phnum as usize * black_header.black_phentsize as usize);
    if black_ph_end > black_len {
        return -8;
    }

    0
}

#[no_mangle]
pub extern "C" fn black_elf_get_entry_rust(black_data: *const u8, black_len: usize) -> u32 {
    if black_data.is_null() || black_len < 52 {
        return 0;
    }
    let black_header = unsafe { &*(black_data as *const BlackElfHeader) };
    black_header.black_entry
}

#[no_mangle]
pub extern "C" fn black_elf_get_phnum_rust(black_data: *const u8, black_len: usize) -> u16 {
    if black_data.is_null() || black_len < 52 { return 0; }
    let black_header = unsafe { &*(black_data as *const BlackElfHeader) };
    black_header.black_phnum
}

#[no_mangle]
pub extern "C" fn black_elf_get_shnum_rust(black_data: *const u8, black_len: usize) -> u16 {
    if black_data.is_null() || black_len < 52 { return 0; }
    let black_header = unsafe { &*(black_data as *const BlackElfHeader) };
    black_header.black_shnum
}

#[no_mangle]
pub unsafe extern "C" fn black_elf_get_load_segment(
    black_data: *const u8,
    black_len: usize,
    black_idx: u16,
    black_vaddr: &mut u32,
    black_memsz: &mut u32,
    black_filesz: &mut u32,
    black_offset: &mut u32,
) -> i32 {
    if black_data.is_null() || black_len < 52 { return -1; }
    let black_header = &*(black_data as *const BlackElfHeader);
    if black_idx >= black_header.black_phnum { return -2; }

    let black_ph_offset = black_header.black_phoff as usize
        + black_idx as usize * black_header.black_phentsize as usize;
    if black_ph_offset + 32 > black_len { return -3; }

    let black_ph = &*(black_data.add(black_ph_offset) as *const BlackElfProgramHeader);
    if black_ph.black_type != BLACK_PT_LOAD { return -4; }

    *black_vaddr = black_ph.black_vaddr;
    *black_memsz = black_ph.black_memsz;
    *black_filesz = black_ph.black_filesz;
    *black_offset = black_ph.black_offset;
    0
}

#[no_mangle]
pub unsafe extern "C" fn black_elf_analyze(
    black_data: *const u8,
    black_len: usize,
    black_info: &mut BlackElfInfo,
) -> i32 {
    black_info.black_valid = 0;
    black_info.black_text_size = 0;
    black_info.black_data_size = 0;
    black_info.black_bss_size = 0;

    let black_verify = black_elf_verify_rust(black_data, black_len);
    if black_verify != 0 { return black_verify; }

    let black_header = &*(black_data as *const BlackElfHeader);
    black_info.black_entry = black_header.black_entry;
    black_info.black_phnum = black_header.black_phnum;
    black_info.black_shnum = black_header.black_shnum;
    black_info.black_type = black_header.black_type;
    black_info.black_machine = black_header.black_machine;

    for black_i in 0..black_header.black_shnum as usize {
        let black_sh_offset = black_header.black_shoff as usize
            + black_i * black_header.black_shentsize as usize;
        if black_sh_offset + 40 > black_len { continue; }

        let black_sh = &*(black_data.add(black_sh_offset) as *const BlackElfSectionHeader);

        if black_sh.black_type == BLACK_SHT_PROGBITS && (black_sh.black_flags & BLACK_SHF_EXECINSTR) != 0 {
            black_info.black_text_size += black_sh.black_size;
        } else if black_sh.black_type == BLACK_SHT_PROGBITS && (black_sh.black_flags & BLACK_SHF_WRITE) != 0 {
            black_info.black_data_size += black_sh.black_size;
        } else if black_sh.black_type == BLACK_SHT_NOBITS {
            black_info.black_bss_size += black_sh.black_size;
        }
    }

    black_info.black_valid = 1;
    0
}

#[no_mangle]
pub unsafe extern "C" fn black_elf_validate_segments(
    black_data: *const u8,
    black_len: usize,
) -> i32 {
    if black_data.is_null() || black_len < 52 { return -1; }
    let black_header = &*(black_data as *const BlackElfHeader);
    let mut black_load_count = 0i32;

    for black_i in 0..black_header.black_phnum as usize {
        let black_ph_offset = black_header.black_phoff as usize
            + black_i * black_header.black_phentsize as usize;
        if black_ph_offset + 32 > black_len { return -2; }

        let black_ph = &*(black_data.add(black_ph_offset) as *const BlackElfProgramHeader);
        if black_ph.black_type == BLACK_PT_NULL { continue; }
        if black_ph.black_type == BLACK_PT_LOAD {
            if black_ph.black_offset as usize + black_ph.black_filesz as usize > black_len {
                return -3;
            }
            if black_ph.black_memsz < black_ph.black_filesz {
                return -4;
            }
            if black_ph.black_vaddr < 0x08048000 {
                return -5;
            }
            black_load_count += 1;
        }
    }

    if black_load_count == 0 { return -6; }
    black_load_count
}

#[panic_handler]
fn black_panic(_black_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
