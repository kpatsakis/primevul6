static void ok_jpg_idct_16x8(const int16_t *input, uint8_t *output) {
    int temp[8 * 8];
    ok_jpg_idct_1d_col_8(input, temp);
    ok_jpg_idct_1d_row_16(8, temp, output);
}