static inline int ok_jpg_peek_bits(ok_jpg_decoder *decoder, int num_bits) {
    return (int)((decoder->input_buffer_bits >> (decoder->input_buffer_bit_count - num_bits)) &
                 ((1 << num_bits) - 1));
}