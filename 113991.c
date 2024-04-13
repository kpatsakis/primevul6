static bool ok_jpg_read_dri(ok_jpg_decoder *decoder) {
    // JPEG spec: Table B.7
    uint8_t buffer[4];
    if (!ok_read(decoder, buffer, sizeof(buffer))) {
        return false;
    }
    int length = readBE16(buffer) - 2;
    if (length != 2) {
        ok_jpg_error(decoder->jpg, OK_JPG_ERROR_INVALID, "Invalid DRI segment length");
        return false;
    } else {
        decoder->restart_intervals = readBE16(buffer + 2);
        return true;
    }
}