#![no_std]
#![allow(non_camel_case_types)]

#[repr(C)]
pub struct black_ring_buffer {
    black_data: [u8; 4096],
    black_head: usize,
    black_tail: usize,
    black_count: usize,
}

impl black_ring_buffer {
    pub fn black_new() -> Self {
        black_ring_buffer {
            black_data: [0u8; 4096],
            black_head: 0,
            black_tail: 0,
            black_count: 0,
        }
    }

    pub fn black_push(&mut self, black_byte: u8) -> bool {
        if self.black_count >= 4096 {
            return false;
        }
        self.black_data[self.black_head] = black_byte;
        self.black_head = (self.black_head + 1) % 4096;
        self.black_count += 1;
        true
    }

    pub fn black_pop(&mut self) -> Option<u8> {
        if self.black_count == 0 {
            return None;
        }
        let black_byte = self.black_data[self.black_tail];
        self.black_tail = (self.black_tail + 1) % 4096;
        self.black_count -= 1;
        Some(black_byte)
    }

    pub fn black_peek(&self) -> Option<u8> {
        if self.black_count == 0 { None }
        else { Some(self.black_data[self.black_tail]) }
    }

    pub fn black_is_empty(&self) -> bool { self.black_count == 0 }
    pub fn black_is_full(&self) -> bool { self.black_count >= 4096 }
    pub fn black_len(&self) -> usize { self.black_count }
    pub fn black_capacity(&self) -> usize { 4096 }

    pub fn black_clear(&mut self) {
        self.black_head = 0;
        self.black_tail = 0;
        self.black_count = 0;
    }
}

#[no_mangle]
pub extern "C" fn black_ring_create() -> black_ring_buffer {
    black_ring_buffer::black_new()
}

#[no_mangle]
pub extern "C" fn black_ring_push(black_rb: &mut black_ring_buffer, black_byte: u8) -> i32 {
    if black_rb.black_push(black_byte) { 0 } else { -1 }
}

#[no_mangle]
pub extern "C" fn black_ring_pop(black_rb: &mut black_ring_buffer, black_out: &mut u8) -> i32 {
    match black_rb.black_pop() {
        Some(black_b) => { *black_out = black_b; 0 }
        None => -1
    }
}

#[no_mangle]
pub extern "C" fn black_ring_peek(black_rb: &black_ring_buffer, black_out: &mut u8) -> i32 {
    match black_rb.black_peek() {
        Some(black_b) => { *black_out = black_b; 0 }
        None => -1
    }
}

#[no_mangle]
pub extern "C" fn black_ring_len(black_rb: &black_ring_buffer) -> usize {
    black_rb.black_len()
}

#[no_mangle]
pub extern "C" fn black_ring_clear(black_rb: &mut black_ring_buffer) {
    black_rb.black_clear();
}

#[repr(C)]
pub struct black_crc32_ctx {
    black_crc: u32,
}

#[no_mangle]
pub extern "C" fn black_crc32_init(black_ctx: &mut black_crc32_ctx) {
    black_ctx.black_crc = 0xFFFFFFFF;
}

#[no_mangle]
pub unsafe extern "C" fn black_crc32_update(
    black_ctx: &mut black_crc32_ctx,
    black_data: *const u8,
    black_len: usize,
) {
    for black_i in 0..black_len {
        let black_byte = *black_data.add(black_i);
        black_ctx.black_crc ^= black_byte as u32;
        for _ in 0..8 {
            if black_ctx.black_crc & 1 != 0 {
                black_ctx.black_crc = (black_ctx.black_crc >> 1) ^ 0xEDB88320;
            } else {
                black_ctx.black_crc >>= 1;
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn black_crc32_final(black_ctx: &black_crc32_ctx) -> u32 {
    black_ctx.black_crc ^ 0xFFFFFFFF
}

#[no_mangle]
pub extern "C" fn black_secure_memzero(black_ptr: *mut u8, black_len: usize) {
    for black_i in 0..black_len {
        unsafe {
            core::ptr::write_volatile(black_ptr.add(black_i), 0);
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn black_secure_memcmp(
    black_a: *const u8,
    black_b: *const u8,
    black_len: usize,
) -> i32 {
    let mut black_diff: u8 = 0;
    for black_i in 0..black_len {
        black_diff |= *black_a.add(black_i) ^ *black_b.add(black_i);
    }
    black_diff as i32
}

static mut BLACK_XORSHIFT_STATE: u64 = 0x4242424242424242;

#[no_mangle]
pub extern "C" fn black_secure_random_seed(black_seed: u32) {
    unsafe {
        BLACK_XORSHIFT_STATE = if black_seed == 0 { 1 } else { black_seed as u64 };
    }
}

#[no_mangle]
pub extern "C" fn black_secure_random() -> u32 {
    unsafe {
        BLACK_XORSHIFT_STATE ^= BLACK_XORSHIFT_STATE << 13;
        BLACK_XORSHIFT_STATE ^= BLACK_XORSHIFT_STATE >> 7;
        BLACK_XORSHIFT_STATE ^= BLACK_XORSHIFT_STATE << 17;
        BLACK_XORSHIFT_STATE as u32
    }
}

#[no_mangle]
pub extern "C" fn black_secure_random_range(black_min: u32, black_max: u32) -> u32 {
    if black_min >= black_max { return black_min; }
    let black_range = black_max - black_min;
    black_min + (black_secure_random() % black_range)
}

#[repr(C)]
pub struct black_bitset {
    black_bits: [u32; 1024],
    black_size: usize,
}

#[no_mangle]
pub extern "C" fn black_bitset_init(black_bs: &mut black_bitset, black_size: usize) {
    black_bs.black_size = if black_size > 32768 { 32768 } else { black_size };
    for black_i in 0..1024 {
        black_bs.black_bits[black_i] = 0;
    }
}

#[no_mangle]
pub extern "C" fn black_bitset_set(black_bs: &mut black_bitset, black_idx: usize) {
    if black_idx < black_bs.black_size {
        black_bs.black_bits[black_idx / 32] |= 1 << (black_idx % 32);
    }
}

#[no_mangle]
pub extern "C" fn black_bitset_clear(black_bs: &mut black_bitset, black_idx: usize) {
    if black_idx < black_bs.black_size {
        black_bs.black_bits[black_idx / 32] &= !(1 << (black_idx % 32));
    }
}

#[no_mangle]
pub extern "C" fn black_bitset_test(black_bs: &black_bitset, black_idx: usize) -> i32 {
    if black_idx < black_bs.black_size {
        if black_bs.black_bits[black_idx / 32] & (1 << (black_idx % 32)) != 0 { 1 } else { 0 }
    } else {
        0
    }
}

#[no_mangle]
pub extern "C" fn black_bitset_first_free(black_bs: &black_bitset) -> i32 {
    for black_i in 0..black_bs.black_size {
        if black_bs.black_bits[black_i / 32] & (1 << (black_i % 32)) == 0 {
            return black_i as i32;
        }
    }
    -1
}

#[panic_handler]
fn black_panic(_black_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
