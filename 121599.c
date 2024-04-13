unsigned char hex_pair_to_byte(char big, char little) {
    return hex_to_nibble(big) * 16 + hex_to_nibble(little);
}