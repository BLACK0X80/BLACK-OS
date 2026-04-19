#ifndef _BLACK_WAV_H
#define _BLACK_WAV_H
#include <stdint.h>
int black_wav_parse(void *black_data, uint32_t black_len, uint32_t *black_rate, uint16_t *black_channels, uint16_t *black_bits, void **black_audio_data, uint32_t *black_audio_len);
#endif
