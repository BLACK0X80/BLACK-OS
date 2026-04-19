#![no_std]
#![allow(non_camel_case_types)]

const BLACK_SHA256_K: [u32; 64] = [
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
];

#[repr(C)]
pub struct black_sha256_ctx {
    black_state: [u32; 8],
    black_buffer: [u8; 64],
    black_buflen: usize,
    black_total: u64,
}

fn black_rotr(black_x: u32, black_n: u32) -> u32 {
    (black_x >> black_n) | (black_x << (32 - black_n))
}

fn black_ch(black_x: u32, black_y: u32, black_z: u32) -> u32 {
    (black_x & black_y) ^ (!black_x & black_z)
}

fn black_maj(black_x: u32, black_y: u32, black_z: u32) -> u32 {
    (black_x & black_y) ^ (black_x & black_z) ^ (black_y & black_z)
}

fn black_sigma0(black_x: u32) -> u32 {
    black_rotr(black_x, 2) ^ black_rotr(black_x, 13) ^ black_rotr(black_x, 22)
}

fn black_sigma1(black_x: u32) -> u32 {
    black_rotr(black_x, 6) ^ black_rotr(black_x, 11) ^ black_rotr(black_x, 25)
}

fn black_gamma0(black_x: u32) -> u32 {
    black_rotr(black_x, 7) ^ black_rotr(black_x, 18) ^ (black_x >> 3)
}

fn black_gamma1(black_x: u32) -> u32 {
    black_rotr(black_x, 17) ^ black_rotr(black_x, 19) ^ (black_x >> 10)
}

fn black_sha256_transform(black_state: &mut [u32; 8], black_block: &[u8; 64]) {
    let mut black_w = [0u32; 64];

    for black_i in 0..16 {
        black_w[black_i] = ((black_block[black_i * 4] as u32) << 24)
            | ((black_block[black_i * 4 + 1] as u32) << 16)
            | ((black_block[black_i * 4 + 2] as u32) << 8)
            | (black_block[black_i * 4 + 3] as u32);
    }

    for black_i in 16..64 {
        black_w[black_i] = black_gamma1(black_w[black_i - 2])
            .wrapping_add(black_w[black_i - 7])
            .wrapping_add(black_gamma0(black_w[black_i - 15]))
            .wrapping_add(black_w[black_i - 16]);
    }

    let mut black_a = black_state[0];
    let mut black_b = black_state[1];
    let mut black_c = black_state[2];
    let mut black_d = black_state[3];
    let mut black_e = black_state[4];
    let mut black_f = black_state[5];
    let mut black_g = black_state[6];
    let mut black_h = black_state[7];

    for black_i in 0..64 {
        let black_t1 = black_h
            .wrapping_add(black_sigma1(black_e))
            .wrapping_add(black_ch(black_e, black_f, black_g))
            .wrapping_add(BLACK_SHA256_K[black_i])
            .wrapping_add(black_w[black_i]);
        let black_t2 = black_sigma0(black_a).wrapping_add(black_maj(black_a, black_b, black_c));

        black_h = black_g;
        black_g = black_f;
        black_f = black_e;
        black_e = black_d.wrapping_add(black_t1);
        black_d = black_c;
        black_c = black_b;
        black_b = black_a;
        black_a = black_t1.wrapping_add(black_t2);
    }

    black_state[0] = black_state[0].wrapping_add(black_a);
    black_state[1] = black_state[1].wrapping_add(black_b);
    black_state[2] = black_state[2].wrapping_add(black_c);
    black_state[3] = black_state[3].wrapping_add(black_d);
    black_state[4] = black_state[4].wrapping_add(black_e);
    black_state[5] = black_state[5].wrapping_add(black_f);
    black_state[6] = black_state[6].wrapping_add(black_g);
    black_state[7] = black_state[7].wrapping_add(black_h);
}

#[no_mangle]
pub extern "C" fn black_sha256_init(black_ctx: &mut black_sha256_ctx) {
    black_ctx.black_state = [
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
    ];
    black_ctx.black_buflen = 0;
    black_ctx.black_total = 0;
    black_ctx.black_buffer = [0u8; 64];
}

#[no_mangle]
pub unsafe extern "C" fn black_sha256_update(
    black_ctx: &mut black_sha256_ctx,
    black_data: *const u8,
    black_len: usize,
) {
    let mut black_offset = 0usize;
    let mut black_remaining = black_len;

    while black_remaining > 0 {
        let black_space = 64 - black_ctx.black_buflen;
        let black_copy = if black_remaining < black_space { black_remaining } else { black_space };

        for black_i in 0..black_copy {
            black_ctx.black_buffer[black_ctx.black_buflen + black_i] =
                *black_data.add(black_offset + black_i);
        }

        black_ctx.black_buflen += black_copy;
        black_offset += black_copy;
        black_remaining -= black_copy;
        black_ctx.black_total += black_copy as u64;

        if black_ctx.black_buflen == 64 {
            let mut black_block = [0u8; 64];
            black_block.copy_from_slice(&black_ctx.black_buffer);
            black_sha256_transform(&mut black_ctx.black_state, &black_block);
            black_ctx.black_buflen = 0;
        }
    }
}

#[no_mangle]
pub extern "C" fn black_sha256_final(black_ctx: &mut black_sha256_ctx, black_hash: &mut [u8; 32]) {
    let black_bits = black_ctx.black_total * 8;

    black_ctx.black_buffer[black_ctx.black_buflen] = 0x80;
    black_ctx.black_buflen += 1;

    if black_ctx.black_buflen > 56 {
        while black_ctx.black_buflen < 64 {
            black_ctx.black_buffer[black_ctx.black_buflen] = 0;
            black_ctx.black_buflen += 1;
        }
        let mut black_block = [0u8; 64];
        black_block.copy_from_slice(&black_ctx.black_buffer);
        black_sha256_transform(&mut black_ctx.black_state, &black_block);
        black_ctx.black_buflen = 0;
    }

    while black_ctx.black_buflen < 56 {
        black_ctx.black_buffer[black_ctx.black_buflen] = 0;
        black_ctx.black_buflen += 1;
    }

    for black_i in 0..8 {
        black_ctx.black_buffer[56 + black_i] = (black_bits >> (56 - black_i * 8)) as u8;
    }

    let mut black_block = [0u8; 64];
    black_block.copy_from_slice(&black_ctx.black_buffer);
    black_sha256_transform(&mut black_ctx.black_state, &black_block);

    for black_i in 0..8 {
        black_hash[black_i * 4] = (black_ctx.black_state[black_i] >> 24) as u8;
        black_hash[black_i * 4 + 1] = (black_ctx.black_state[black_i] >> 16) as u8;
        black_hash[black_i * 4 + 2] = (black_ctx.black_state[black_i] >> 8) as u8;
        black_hash[black_i * 4 + 3] = black_ctx.black_state[black_i] as u8;
    }
}

#[no_mangle]
pub unsafe extern "C" fn black_sha256_hash(
    black_data: *const u8,
    black_len: usize,
    black_out: &mut [u8; 32],
) {
    let mut black_ctx = black_sha256_ctx {
        black_state: [0; 8],
        black_buffer: [0; 64],
        black_buflen: 0,
        black_total: 0,
    };
    black_sha256_init(&mut black_ctx);
    black_sha256_update(&mut black_ctx, black_data, black_len);
    black_sha256_final(&mut black_ctx, black_out);
}

#[panic_handler]
fn black_panic(_black_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
