unsigned char hex_to_nibble(char val) {
    if (val >= 'A' && val <= 'F') {
        return val - 'A' + 10;
    }
    if (val >= 'a' && val <= 'f') {
        return val - 'a' + 10;
    }
    return val - '0';
}