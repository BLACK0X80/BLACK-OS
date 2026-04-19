#![no_std]
#![allow(non_camel_case_types)]

const BLACK_SLAB_SIZES: [usize; 8] = [8, 16, 32, 64, 128, 256, 512, 1024];
const BLACK_SLAB_ENTRIES: usize = 256;

#[repr(C)]
pub struct black_slab {
    black_bitmap: [u32; 8],
    black_obj_size: usize,
    black_base: usize,
    black_count: usize,
}

impl black_slab {
    pub fn black_new(black_obj_size: usize, black_base: usize) -> Self {
        black_slab {
            black_bitmap: [0u32; 8],
            black_obj_size,
            black_base,
            black_count: 0,
        }
    }

    pub fn black_alloc(&mut self) -> Option<usize> {
        for black_i in 0..BLACK_SLAB_ENTRIES {
            let black_word = black_i / 32;
            let black_bit = black_i % 32;
            if self.black_bitmap[black_word] & (1 << black_bit) == 0 {
                self.black_bitmap[black_word] |= 1 << black_bit;
                self.black_count += 1;
                return Some(self.black_base + black_i * self.black_obj_size);
            }
        }
        None
    }

    pub fn black_free(&mut self, black_addr: usize) -> bool {
        if black_addr < self.black_base {
            return false;
        }
        let black_offset = black_addr - self.black_base;
        if black_offset % self.black_obj_size != 0 {
            return false;
        }
        let black_idx = black_offset / self.black_obj_size;
        if black_idx >= BLACK_SLAB_ENTRIES {
            return false;
        }
        let black_word = black_idx / 32;
        let black_bit = black_idx % 32;
        if self.black_bitmap[black_word] & (1 << black_bit) == 0 {
            return false;
        }
        self.black_bitmap[black_word] &= !(1 << black_bit);
        self.black_count -= 1;
        true
    }

    pub fn black_usage(&self) -> usize { self.black_count }
    pub fn black_capacity(&self) -> usize { BLACK_SLAB_ENTRIES }
}

#[no_mangle]
pub extern "C" fn black_slab_create(black_obj_size: usize, black_base: usize) -> black_slab {
    black_slab::black_new(black_obj_size, black_base)
}

#[no_mangle]
pub extern "C" fn black_slab_alloc(black_s: &mut black_slab) -> usize {
    match black_s.black_alloc() {
        Some(black_addr) => black_addr,
        None => 0,
    }
}

#[no_mangle]
pub extern "C" fn black_slab_free(black_s: &mut black_slab, black_addr: usize) -> i32 {
    if black_s.black_free(black_addr) { 0 } else { -1 }
}

#[no_mangle]
pub extern "C" fn black_slab_usage(black_s: &black_slab) -> usize {
    black_s.black_usage()
}

#[repr(C)]
pub struct black_pool_stats {
    pub black_alloc_count: u64,
    pub black_free_count: u64,
    pub black_alloc_bytes: u64,
    pub black_free_bytes: u64,
    pub black_peak_usage: u64,
    pub black_current_usage: u64,
    pub black_fragmentation: u32,
}

static mut BLACK_POOL_STATS: black_pool_stats = black_pool_stats {
    black_alloc_count: 0,
    black_free_count: 0,
    black_alloc_bytes: 0,
    black_free_bytes: 0,
    black_peak_usage: 0,
    black_current_usage: 0,
    black_fragmentation: 0,
};

#[no_mangle]
pub extern "C" fn black_pool_stats_record_alloc(black_size: usize) {
    unsafe {
        BLACK_POOL_STATS.black_alloc_count += 1;
        BLACK_POOL_STATS.black_alloc_bytes += black_size as u64;
        BLACK_POOL_STATS.black_current_usage += black_size as u64;
        if BLACK_POOL_STATS.black_current_usage > BLACK_POOL_STATS.black_peak_usage {
            BLACK_POOL_STATS.black_peak_usage = BLACK_POOL_STATS.black_current_usage;
        }
    }
}

#[no_mangle]
pub extern "C" fn black_pool_stats_record_free(black_size: usize) {
    unsafe {
        BLACK_POOL_STATS.black_free_count += 1;
        BLACK_POOL_STATS.black_free_bytes += black_size as u64;
        if BLACK_POOL_STATS.black_current_usage >= black_size as u64 {
            BLACK_POOL_STATS.black_current_usage -= black_size as u64;
        }
    }
}

#[no_mangle]
pub extern "C" fn black_pool_stats_get(black_out: &mut black_pool_stats) {
    unsafe {
        black_out.black_alloc_count = BLACK_POOL_STATS.black_alloc_count;
        black_out.black_free_count = BLACK_POOL_STATS.black_free_count;
        black_out.black_alloc_bytes = BLACK_POOL_STATS.black_alloc_bytes;
        black_out.black_free_bytes = BLACK_POOL_STATS.black_free_bytes;
        black_out.black_peak_usage = BLACK_POOL_STATS.black_peak_usage;
        black_out.black_current_usage = BLACK_POOL_STATS.black_current_usage;
        black_out.black_fragmentation = BLACK_POOL_STATS.black_fragmentation;
    }
}

#[no_mangle]
pub unsafe extern "C" fn black_mem_guard_check(
    black_ptr: *const u8,
    black_size: usize,
    black_guard_val: u8,
) -> i32 {
    if black_ptr.is_null() { return -1; }
    let black_pre = black_ptr.sub(4);
    for black_i in 0..4 {
        if *black_pre.add(black_i) != black_guard_val {
            return -2;
        }
    }
    let black_post = black_ptr.add(black_size);
    for black_i in 0..4 {
        if *black_post.add(black_i) != black_guard_val {
            return -3;
        }
    }
    0
}

#[no_mangle]
pub unsafe extern "C" fn black_mem_pattern_fill(
    black_ptr: *mut u8,
    black_size: usize,
    black_pattern: u8,
) {
    for black_i in 0..black_size {
        core::ptr::write_volatile(black_ptr.add(black_i), black_pattern);
    }
}

#[no_mangle]
pub unsafe extern "C" fn black_mem_poison_check(
    black_ptr: *const u8,
    black_size: usize,
    black_poison: u8,
) -> i32 {
    for black_i in 0..black_size {
        if *black_ptr.add(black_i) != black_poison {
            return black_i as i32;
        }
    }
    -1
}

fn black_slab_size_class(black_size: usize) -> Option<usize> {
    for black_i in 0..BLACK_SLAB_SIZES.len() {
        if black_size <= BLACK_SLAB_SIZES[black_i] {
            return Some(black_i);
        }
    }
    None
}

#[no_mangle]
pub extern "C" fn black_get_slab_class(black_size: usize) -> i32 {
    match black_slab_size_class(black_size) {
        Some(black_cls) => BLACK_SLAB_SIZES[black_cls] as i32,
        None => -1,
    }
}

#[panic_handler]
fn black_panic(_black_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
