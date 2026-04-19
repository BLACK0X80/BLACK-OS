#![no_std]
#![allow(non_camel_case_types)]

#[repr(C, packed)]
pub struct black_ethernet_frame {
    pub black_dst_mac: [u8; 6],
    pub black_src_mac: [u8; 6],
    pub black_ethertype: u16,
}

#[repr(C, packed)]
pub struct black_ipv4_header {
    pub black_ver_ihl: u8,
    pub black_tos: u8,
    pub black_total_len: u16,
    pub black_id: u16,
    pub black_flags_frag: u16,
    pub black_ttl: u8,
    pub black_protocol: u8,
    pub black_checksum: u16,
    pub black_src_ip: u32,
    pub black_dst_ip: u32,
}

#[repr(C, packed)]
pub struct black_tcp_header {
    pub black_src_port: u16,
    pub black_dst_port: u16,
    pub black_seq: u32,
    pub black_ack: u32,
    pub black_offset_flags: u16,
    pub black_window: u16,
    pub black_checksum: u16,
    pub black_urgent: u16,
}

#[repr(C, packed)]
pub struct black_udp_header {
    pub black_src_port: u16,
    pub black_dst_port: u16,
    pub black_length: u16,
    pub black_checksum: u16,
}

#[repr(C)]
pub struct black_packet_info {
    pub black_valid: i32,
    pub black_ethertype: u16,
    pub black_ip_version: u8,
    pub black_ip_protocol: u8,
    pub black_src_ip: u32,
    pub black_dst_ip: u32,
    pub black_src_port: u16,
    pub black_dst_port: u16,
    pub black_payload_offset: u16,
    pub black_payload_len: u16,
    pub black_ttl: u8,
    pub black_flags: u8,
}

fn black_ntohs(black_val: u16) -> u16 {
    (black_val >> 8) | (black_val << 8)
}

fn black_ntohl(black_val: u32) -> u32 {
    ((black_val & 0xFF) << 24)
        | ((black_val & 0xFF00) << 8)
        | ((black_val & 0xFF0000) >> 8)
        | ((black_val & 0xFF000000) >> 24)
}

#[no_mangle]
pub unsafe extern "C" fn black_validate_ethernet(
    black_data: *const u8,
    black_len: usize,
) -> i32 {
    if black_data.is_null() || black_len < 14 { return -1; }
    let black_frame = &*(black_data as *const black_ethernet_frame);
    let black_etype = black_ntohs(black_frame.black_ethertype);
    if black_etype != 0x0800 && black_etype != 0x0806 && black_etype != 0x86DD {
        return -2;
    }
    0
}

#[no_mangle]
pub unsafe extern "C" fn black_validate_ipv4(
    black_data: *const u8,
    black_len: usize,
) -> i32 {
    if black_data.is_null() || black_len < 20 { return -1; }
    let black_hdr = &*(black_data as *const black_ipv4_header);
    let black_ver = (black_hdr.black_ver_ihl >> 4) & 0xF;
    let black_ihl = (black_hdr.black_ver_ihl & 0xF) as usize * 4;

    if black_ver != 4 { return -2; }
    if black_ihl < 20 { return -3; }
    if black_ihl > black_len { return -4; }

    let black_total = black_ntohs(black_hdr.black_total_len) as usize;
    if black_total < black_ihl { return -5; }
    if black_total > black_len { return -6; }

    if black_hdr.black_ttl == 0 { return -7; }

    let mut black_sum: u32 = 0;
    let black_words = black_ihl / 2;
    for black_i in 0..black_words {
        let black_word = ((*black_data.add(black_i * 2) as u16) << 8)
            | (*black_data.add(black_i * 2 + 1) as u16);
        black_sum += black_word as u32;
    }
    while black_sum > 0xFFFF {
        black_sum = (black_sum & 0xFFFF) + (black_sum >> 16);
    }
    if black_sum != 0xFFFF { return -8; }

    0
}

#[no_mangle]
pub unsafe extern "C" fn black_validate_tcp(
    black_data: *const u8,
    black_len: usize,
) -> i32 {
    if black_data.is_null() || black_len < 20 { return -1; }
    let black_hdr = &*(black_data as *const black_tcp_header);
    let black_offset = ((black_ntohs(black_hdr.black_offset_flags) >> 12) & 0xF) as usize * 4;
    if black_offset < 20 { return -2; }
    if black_offset > black_len { return -3; }
    0
}

#[no_mangle]
pub unsafe extern "C" fn black_validate_udp(
    black_data: *const u8,
    black_len: usize,
) -> i32 {
    if black_data.is_null() || black_len < 8 { return -1; }
    let black_hdr = &*(black_data as *const black_udp_header);
    let black_udp_len = black_ntohs(black_hdr.black_length) as usize;
    if black_udp_len < 8 { return -2; }
    if black_udp_len > black_len { return -3; }
    0
}

#[no_mangle]
pub unsafe extern "C" fn black_parse_packet(
    black_data: *const u8,
    black_len: usize,
    black_info: &mut black_packet_info,
) -> i32 {
    black_info.black_valid = 0;
    if black_data.is_null() || black_len < 14 { return -1; }

    let black_frame = &*(black_data as *const black_ethernet_frame);
    black_info.black_ethertype = black_ntohs(black_frame.black_ethertype);

    if black_info.black_ethertype != 0x0800 { return -2; }

    let black_ip_data = black_data.add(14);
    let black_ip_len = black_len - 14;
    if black_ip_len < 20 { return -3; }

    let black_ip_hdr = &*(black_ip_data as *const black_ipv4_header);
    let black_ihl = (black_ip_hdr.black_ver_ihl & 0xF) as usize * 4;

    black_info.black_ip_version = 4;
    black_info.black_ip_protocol = black_ip_hdr.black_protocol;
    black_info.black_src_ip = black_ntohl(black_ip_hdr.black_src_ip);
    black_info.black_dst_ip = black_ntohl(black_ip_hdr.black_dst_ip);
    black_info.black_ttl = black_ip_hdr.black_ttl;

    let black_transport_data = black_ip_data.add(black_ihl);
    let black_transport_len = black_ip_len - black_ihl;

    match black_ip_hdr.black_protocol {
        6 => {
            if black_transport_len < 20 { return -4; }
            let black_tcp = &*(black_transport_data as *const black_tcp_header);
            black_info.black_src_port = black_ntohs(black_tcp.black_src_port);
            black_info.black_dst_port = black_ntohs(black_tcp.black_dst_port);
            let black_tcp_off = ((black_ntohs(black_tcp.black_offset_flags) >> 12) & 0xF) as u16 * 4;
            black_info.black_payload_offset = 14 + black_ihl as u16 + black_tcp_off;
            black_info.black_flags = (black_ntohs(black_tcp.black_offset_flags) & 0x3F) as u8;
        }
        17 => {
            if black_transport_len < 8 { return -5; }
            let black_udp = &*(black_transport_data as *const black_udp_header);
            black_info.black_src_port = black_ntohs(black_udp.black_src_port);
            black_info.black_dst_port = black_ntohs(black_udp.black_dst_port);
            black_info.black_payload_offset = 14 + black_ihl as u16 + 8;
            black_info.black_payload_len = black_ntohs(black_udp.black_length) - 8;
        }
        _ => {}
    }

    black_info.black_valid = 1;
    0
}

#[no_mangle]
pub unsafe extern "C" fn black_compute_ip_checksum(
    black_data: *const u8,
    black_len: usize,
) -> u16 {
    let mut black_sum: u32 = 0;
    let black_words = black_len / 2;
    for black_i in 0..black_words {
        let black_word = ((*black_data.add(black_i * 2) as u16) << 8)
            | (*black_data.add(black_i * 2 + 1) as u16);
        black_sum += black_word as u32;
    }
    if black_len % 2 == 1 {
        black_sum += (*black_data.add(black_len - 1) as u32) << 8;
    }
    while black_sum > 0xFFFF {
        black_sum = (black_sum & 0xFFFF) + (black_sum >> 16);
    }
    !(black_sum as u16)
}

#[panic_handler]
fn black_panic(_black_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
