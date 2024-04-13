static void ok_jpg_decode_block_progressive(ok_jpg_decoder *decoder, ok_jpg_component *c,
                                            int16_t *block) {
    int k = decoder->scan_start;
    const int k_end = decoder->scan_end;
    const int scale = decoder->scan_scale;

    memset(block + k, 0, (size_t)(k_end - k + 1) * sizeof(*block));

    // Decode DC coefficients - F.2.2.1
    if (k == 0) {
        ok_jpg_huffman_table *dc = decoder->dc_huffman_tables + c->Td;
        uint8_t t = ok_jpg_huffman_decode(decoder, dc);
        if (t > 0) {
            int diff = ok_jpg_load_next_bits(decoder, t);
            c->pred += ok_jpg_extend(diff, t) << scale;
        }
        block[0] = c->pred;
        k++;
    }

    // Decode AC coefficients - Figures F.13, F.14, and G.2
    if (c->eob_run > 0) {
        c->eob_run--;
        return;
    }
    ok_jpg_huffman_table *ac = decoder->ac_huffman_tables + c->Ta;
    while (k <= k_end) {
        ok_jpg_load_bits(decoder, 16);
        int code = ok_jpg_peek_bits(decoder, HUFFMAN_LOOKUP_SIZE_BITS);
        uint8_t num_bits = ac->lookup_ac_num_bits[code];
        if (num_bits > 0) {
            ok_jpg_consume_bits(decoder, num_bits);
            uint8_t rs = ac->lookup_val[code];
            int s = rs & 0x0f;
            int r = rs >> 4;
            if (s > 0) {
                k += r;
                block[k] = (int16_t)(ac->lookup_ac_val[code] << scale);
                k++;
            } else {
                if (r != 0x0f) {
                    c->eob_run = ac->lookup_ac_val[code];
                    break;
                }
                k += 16;
            }
        } else {
            uint8_t rs = ok_jpg_huffman_decode(decoder, ac);
            int s = rs & 0x0f;
            int r = rs >> 4;
            if (s > 0) {
                k += r;
                int v = ok_jpg_load_next_bits(decoder, s);
                block[k] = (int16_t)(ok_jpg_extend(v, s) << scale);
                k++;
            } else {
                if (r != 0x0f) {
                    if (r > 0) {
                        int next_bits = ok_jpg_load_next_bits(decoder, r);
                        c->eob_run = (1 << r) + next_bits - 1;
                    }
                    break;
                }
                k += 16;
            }
        }
    }
}