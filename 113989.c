static inline void ok_jpg_dump_bits(ok_jpg_decoder *decoder) {
    decoder->input_buffer_bits = 0;
    decoder->input_buffer_bit_count = 0;
}