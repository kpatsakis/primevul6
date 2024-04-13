static bool ok_jpg_decode_restart_if_needed(ok_jpg_decoder *decoder) {
    if (decoder->restart_intervals_remaining > 0) {
        decoder->restart_intervals_remaining--;

        if (decoder->restart_intervals_remaining == 0) {
            ok_jpg_dump_bits(decoder);
            if (decoder->next_marker != 0) {
                if (decoder->next_marker == 0xD0 + decoder->next_restart) {
                    decoder->next_marker = 0;
                } else {
                    ok_jpg_error(decoder->jpg, OK_JPG_ERROR_INVALID, "Invalid restart marker (1)");
                    return false;
                }
            } else {
                uint8_t buffer[2];
                if (!ok_read(decoder, buffer, 2)) {
                    return false;
                }
                if (!(buffer[0] == 0xff && buffer[1] == 0xD0 + decoder->next_restart)) {
                    ok_jpg_error(decoder->jpg, OK_JPG_ERROR_INVALID, "Invalid restart marker (2)");
                    return false;
                }
            }
            decoder->next_restart = (decoder->next_restart + 1) & 7;

            ok_jpg_decode_restart(decoder);
        }
    }
    return true;
}