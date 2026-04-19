#include <kernel/kernel.h>
#include <string.h>

typedef struct {
    char black_chunk_id[4];
    uint32_t black_chunk_size;
    char black_format[4];
    char black_subchunk1_id[4];
    uint32_t black_subchunk1_size;
    uint16_t black_audio_format;
    uint16_t black_num_channels;
    uint32_t black_sample_rate;
    uint32_t black_byte_rate;
    uint16_t black_block_align;
    uint16_t black_bits_per_sample;
    char black_subchunk2_id[4];
    uint32_t black_subchunk2_size;
} __attribute__((packed)) black_wav_header_t;

int black_wav_parse(void *black_data, uint32_t black_len, uint32_t *black_rate, uint16_t *black_channels, uint16_t *black_bits, void **black_audio_data, uint32_t *black_audio_len) {
    if (black_len < sizeof(black_wav_header_t)) return -1;
    black_wav_header_t *black_hdr = (black_wav_header_t *)black_data;
    if (memcmp(black_hdr->black_chunk_id, "RIFF", 4) != 0) return -1;
    if (memcmp(black_hdr->black_format, "WAVE", 4) != 0) return -1;
    *black_rate = black_hdr->black_sample_rate;
    *black_channels = black_hdr->black_num_channels;
    *black_bits = black_hdr->black_bits_per_sample;
    *black_audio_data = (void *)((uint32_t)black_data + sizeof(black_wav_header_t));
    *black_audio_len = black_hdr->black_subchunk2_size;
    return 0;
}
