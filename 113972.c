static bool ok_seek(ok_jpg_decoder *decoder, long length) {
    if (length == 0) {
        return true;
    } else if (length < 0) {
        ok_jpg_error(decoder->jpg, OK_JPG_ERROR_IO, "Seek error: negative seek unsupported.");
        return false;
    }
    size_t available = (size_t)(decoder->input_buffer_end - decoder->input_buffer_start);

    size_t len = min((size_t)length, available);
    decoder->input_buffer_start += len;
    length -= len;
    if (length > 0) {
        if (decoder->input.seek(decoder->input_user_data, length)) {
            return true;
        } else {
            decoder->eof_found = true;
            ok_jpg_error(decoder->jpg, OK_JPG_ERROR_IO, "Seek error: error calling input function.");
            return false;
        }
    } else {
        return true;
    }
}