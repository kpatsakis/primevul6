static inline void ok_jpg_decode_block_subsequent_scan_rv(ok_jpg_decoder *decoder,
                                                          ok_jpg_component *c,
                                                          ok_jpg_huffman_table *ac,
                                                          int *r, int16_t *v) {
    const int16_t lsb = (int16_t)(1 << decoder->scan_scale);
    uint8_t rs = ok_jpg_huffman_decode(decoder, ac);
    int s = rs & 0x0f;
    *r = rs >> 4;
    if (s == 0) {
        if (*r != 0x0f) {
            if (*r > 0) {
                int next_bits = ok_jpg_load_next_bits(decoder, *r);
                c->eob_run = (1 << *r) + next_bits - 1;
            }
            *r = 64;
        }
    } else {
        int sign = ok_jpg_load_next_bits(decoder, 1);
        *v = sign ? lsb : -lsb;
    }
}