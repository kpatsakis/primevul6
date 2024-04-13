static bool ok_jpg_skip_segment(ok_jpg_decoder *decoder) {
    uint8_t buffer[2];
    if (!ok_read(decoder, buffer, sizeof(buffer))) {
        return false;
    }
    int length = readBE16(buffer) - 2;
    return ok_seek(decoder, length);
}