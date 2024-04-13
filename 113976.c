static bool ok_jpg_scan_to_next_marker(ok_jpg_decoder *decoder) {
    uint8_t buffer[1];
    while (true) {
        if (!ok_read(decoder, buffer, 1)) {
            return false;
        }
        if (buffer[0] == 0xff) {
            if (!ok_read(decoder, buffer, 1)) {
                return false;
            }
            if (buffer[0] != 0 && !(buffer[0] >= 0xD0 && buffer[0] <= 0xD7)) {
                decoder->next_marker = buffer[0];
                return true;
            }
        }
    }
}