static inline uint8_t ok_read_uint8(ok_jpg_decoder *decoder) {
    if (decoder->input_buffer_start == decoder->input_buffer_end) {
        size_t len = decoder->input.read(decoder->input_user_data, decoder->input_buffer,
                                         sizeof(decoder->input_buffer));
        decoder->input_buffer_start = decoder->input_buffer;
        decoder->input_buffer_end = decoder->input_buffer + len;

        if (len == 0) {
            return 0;
        }
    }
    return *decoder->input_buffer_start++;
}