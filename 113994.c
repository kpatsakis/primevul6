static inline void ok_jpg_load_bits(ok_jpg_decoder *decoder, int num_bits) {
    while (decoder->input_buffer_bit_count < num_bits) {
        if (decoder->next_marker != 0) {
            decoder->input_buffer_bits <<= 8;
            decoder->input_buffer_bit_count += 8;
        } else {
            uint8_t b = ok_read_uint8(decoder);
            if (b == 0xff) {
                uint8_t marker = ok_read_uint8(decoder);
                if (marker != 0) {
                    decoder->next_marker = marker;
                    b = 0;
                }
            }
            decoder->input_buffer_bits = (decoder->input_buffer_bits << 8) | b;
            decoder->input_buffer_bit_count += 8;
        }
    }
}