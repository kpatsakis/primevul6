static void ok_jpg_decode_block_subsequent_scan(ok_jpg_decoder *decoder, ok_jpg_component *c,
                                                int16_t *block) {
    int k = decoder->scan_start;
    const int k_end = decoder->scan_end;
    const int16_t lsb = (int16_t)(1 << decoder->scan_scale);

    // Decode DC coefficients - Section G.1.2.1
    if (k == 0) {
        int correction = ok_jpg_load_next_bits(decoder, 1);
        if (correction) {
            block[0] |= lsb;
        }
        if (k_end == 0) {
            return;
        } else {
            k = 1;
        }
    }

    // Decode AC coefficients - Section G.1.2.3
    ok_jpg_huffman_table *ac = decoder->ac_huffman_tables + c->Ta;
    int r;
    int16_t v = 0;
    if (c->eob_run > 0) {
        c->eob_run--;
        r = 64;
    } else {
        ok_jpg_decode_block_subsequent_scan_rv(decoder, c, ac, &r, &v);
    }
    while (k <= k_end) {
        int16_t *coeff = block + k;
        if (*coeff != 0) {
            int correction = ok_jpg_load_next_bits(decoder, 1);
            if (correction) {
                if (*coeff < 0) {
                    *coeff -= lsb;
                } else {
                    *coeff += lsb;
                }
            }
        } else if (r == 0) {
            if (v != 0) {
                *coeff = v;
                v = 0;
            }
            if (k < k_end) {
                ok_jpg_decode_block_subsequent_scan_rv(decoder, c, ac, &r, &v);
            }
        } else {
            r--;
        }
        k++;
    }
}