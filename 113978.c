static void ok_jpg_convert_data_unit_grayscale(const uint8_t *y, uint8_t *output,
                                               const int x_inc, const int y_inc,
                                               const int max_width, const int max_height) {
    for (int v = 0; v < max_height; v++) {
        uint8_t *out = output;
        for (int x = 0; x < max_width; x++) {
            out[0] = y[x];
            out[1] = y[x];
            out[2] = y[x];
            out[3] = 0xff;
            out += x_inc;
        }
        y += C_WIDTH;
        output += y_inc;
    }
}