static void ok_jpg_generate_huffman_table_lookups(ok_jpg_huffman_table *huff, bool is_ac_table) {
    // Look up table for codes that use N bits or less (most of them)
    for (int q = 0; q < HUFFMAN_LOOKUP_SIZE; q++) {
        huff->lookup_num_bits[q] = 0;
        for (uint8_t i = 0; i < HUFFMAN_LOOKUP_SIZE_BITS; i++) {
            uint8_t num_bits = i + 1;
            int code = q >> (HUFFMAN_LOOKUP_SIZE_BITS - num_bits);
            if (code <= huff->maxcode[i]) {
                huff->lookup_num_bits[q] = num_bits;

                int j = huff->valptr[i];
                j += code - huff->mincode[i];
                huff->lookup_val[q] = huff->val[j];
                break;
            }
        }
    }

    if (is_ac_table) {
        // Additional lookup table to get both RS and extended value
        for (int q = 0; q < HUFFMAN_LOOKUP_SIZE; q++) {
            huff->lookup_ac_num_bits[q] = 0;
            int num_bits = huff->lookup_num_bits[q];
            if (num_bits > 0) {
                int rs = huff->lookup_val[q];
                int r = rs >> 4;
                int s = rs & 0x0f;
                int total_bits = num_bits;
                if (s > 0) {
                    total_bits += s;
                } else if (r > 0 && r < 0x0f) {
                    total_bits += r;
                }
                if (total_bits <= HUFFMAN_LOOKUP_SIZE_BITS) {
                    huff->lookup_ac_num_bits[q] = (uint8_t)total_bits;
                    if (s > 0) {
                        int v = (q >> (HUFFMAN_LOOKUP_SIZE_BITS - total_bits)) & ((1 << s) - 1);
                        huff->lookup_ac_val[q] = (int16_t)ok_jpg_extend(v, s);
                    } else if (r > 0 && r < 0x0f) {
                        int v = (q >> (HUFFMAN_LOOKUP_SIZE_BITS - total_bits)) & ((1 << r) - 1);
                        huff->lookup_ac_val[q] = (int16_t)((1 << r) + v - 1);
                    } else {
                        huff->lookup_ac_val[q] = 0;
                    }
                }
            }
        }
    }
}