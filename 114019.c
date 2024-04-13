static inline uint8_t ok_jpg_huffman_decode(ok_jpg_decoder *decoder,
                                            const ok_jpg_huffman_table *table) {
    // JPEG spec: "Decode" (Figure F.16)

    // First, try lookup tables
    ok_jpg_load_bits(decoder, 16);
    int code = ok_jpg_peek_bits(decoder, HUFFMAN_LOOKUP_SIZE_BITS);
    int num_bits = table->lookup_num_bits[code];
    if (num_bits != 0) {
        ok_jpg_consume_bits(decoder, num_bits);
        return table->lookup_val[code];
    }

    // Next, try a code up to 16-bits
    code = ok_jpg_peek_bits(decoder, 16);
    for (int i = HUFFMAN_LOOKUP_SIZE_BITS; i < 16; i++) {
        if (code <= table->maxcode[i]) {
            ok_jpg_consume_bits(decoder, i + 1);
            code >>= (15 - i);
            int j = table->valptr[i];
            j += code - table->mincode[i];
            return table->val[j];
        }
    }

    decoder->huffman_error = true;
    ok_jpg_error(decoder->jpg, OK_JPG_ERROR_INVALID, "Invalid huffman code");
    return 0;
}