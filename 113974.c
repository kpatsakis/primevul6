static inline void ok_jpg_decode_block(ok_jpg_decoder *decoder, ok_jpg_component *c,
                                       int16_t *block) {
    memset(block, 0, 8 * 8 * sizeof(*block));

    const uint8_t *q_table = decoder->q_table[c->Tq];

    // Decode DC coefficients - F.2.2.1
    ok_jpg_huffman_table *dc = decoder->dc_huffman_tables + c->Td;
    uint8_t t = ok_jpg_huffman_decode(decoder, dc);
    if (t > 0) {
        int diff = ok_jpg_load_next_bits(decoder, t);
        c->pred += ok_jpg_extend(diff, t);
    }
    block[0] = c->pred * q_table[0];

    // Decode AC coefficients - Figures F.13 and F.14
    ok_jpg_huffman_table *ac = decoder->ac_huffman_tables + c->Ta;
    int k = 1;
    while (k <= 63) {
        ok_jpg_load_bits(decoder, 16);
        int code = ok_jpg_peek_bits(decoder, HUFFMAN_LOOKUP_SIZE_BITS);
        uint8_t num_bits = ac->lookup_ac_num_bits[code];
        if (num_bits > 0) {
            ok_jpg_consume_bits(decoder, num_bits);
            uint8_t rs = ac->lookup_val[code];
            int s = rs & 0x0f;
            if (s > 0) {
                int r = rs >> 4;
                k += r;
                block[ok_jpg_zig_zag[k]] = ac->lookup_ac_val[code] * q_table[k];
                k++;
            } else {
                if (rs == 0) {
                    break;
                }
                k += 16;
            }
        } else {
            uint8_t rs = ok_jpg_huffman_decode(decoder, ac);
            int s = rs & 0x0f;
            if (s > 0) {
                int r = rs >> 4;
                k += r;
                int v = ok_jpg_load_next_bits(decoder, s);
                block[ok_jpg_zig_zag[k]] = (int16_t)ok_jpg_extend(v, s) * q_table[k];
                k++;
            } else {
                if (rs == 0) {
                    break;
                }
                k += 16;
            }
        }
    }
}