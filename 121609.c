bool is_jpeg_header(Sirikata::Array1d<uint8_t, 2> header) {
    return header[0] == 0xFF && header[1] == 0xD8;
}