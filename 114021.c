static bool ok_read(ok_jpg_decoder *decoder, uint8_t *buffer, size_t length) {
    size_t available = (size_t)(decoder->input_buffer_end - decoder->input_buffer_start);
    if (available) {
        size_t len = min(length, available);
        memcpy(buffer, decoder->input_buffer_start, len);
        decoder->input_buffer_start += len;
        length -= len;
        if (length == 0) {
            return true;
        }
        buffer += len;
    }
    if (decoder->input.read(decoder->input_user_data, buffer, length) == length) {
        return true;
    } else {
        decoder->eof_found = true;
        ok_jpg_error(decoder->jpg, OK_JPG_ERROR_IO, "Read error: error calling input function.");
        return false;
    }
}