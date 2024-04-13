static inline void ok_jpg_consume_bits(ok_jpg_decoder *decoder, int num_bits) {
    decoder->input_buffer_bit_count -= num_bits;
}