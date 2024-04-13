static inline int ok_jpg_load_next_bits(ok_jpg_decoder *decoder, int num_bits) {
    ok_jpg_load_bits(decoder, num_bits);
    int mask = (1 << num_bits) - 1;
    decoder->input_buffer_bit_count -= num_bits;
    return (int)(decoder->input_buffer_bits >> decoder->input_buffer_bit_count) & mask;
}