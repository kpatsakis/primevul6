static void ok_jpg_idct_16x16(const int16_t *input, uint8_t *output) {
    int temp[8 * 16];
    ok_jpg_idct_1d_col_16(input, temp);
    ok_jpg_idct_1d_row_16(16, temp, output);
}