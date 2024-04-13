bool read_jpeg_wrapper(std::vector<std::pair<uint32_t,
                                     uint32_t>> *huff_input_offset,
                       ibytestream *jpg_str_in,
                       Sirikata::Array1d<uint8_t, 2> header,
                       bool is_embedded_jpeg) {
    return read_jpeg(huff_input_offset, jpg_str_in, header, is_embedded_jpeg);
}