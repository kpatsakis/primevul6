static bool ok_jpg_read_dqt(ok_jpg_decoder *decoder) {
    // JPEG spec: Table B.4
    ok_jpg *jpg = decoder->jpg;
    uint8_t buffer[2];
    if (!ok_read(decoder, buffer, sizeof(buffer))) {
        return false;
    }
    int length = readBE16(buffer) - 2;
    while (length >= 65) {
        uint8_t pt = ok_read_uint8(decoder);
        int Pq = pt >> 4;
        int Tq = pt & 0x0f;

        if (Pq == 1) {
            ok_jpg_error(jpg, OK_JPG_ERROR_UNSUPPORTED, "Unsupported JPEG (16-bit q_table)");
            return false;
        }
        if (Pq != 0 || Tq > 3) {
            ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid JPEG (Pq/Tq)");
            return false;
        }
        if (!ok_read(decoder, decoder->q_table[Tq], 64)) {
            return false;
        }
        length -= 65;
    }
    if (length != 0) {
        ok_jpg_error(jpg, OK_JPG_ERROR_INVALID, "Invalid DQT segment length");
        return false;
    } else {
        return true;
    }
}