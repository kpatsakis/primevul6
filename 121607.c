bool hex_to_bin(unsigned char *output, const char *input, size_t output_size) {
    size_t i = 0;
    for (; i < output_size && input[i * 2] && input[i * 2 + 1]; ++i) {
        output[i] = hex_pair_to_byte(input[i * 2], input[i * 2 + 1]);
    }
    return i == output_size;
}