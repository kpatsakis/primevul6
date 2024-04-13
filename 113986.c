static void ok_jpg_convert_data_unit_color(const uint8_t *y, const uint8_t *cb, const uint8_t *cr,
                                           uint8_t *output, bool rgba,
                                           const int x_inc, const int y_inc,
                                           const int max_width, const int max_height) {
    if (rgba) {
        for (int v = 0; v < max_height; v++) {
            uint8_t *out = output;
            for (int x = 0; x < max_width; x++) {
                ok_jpg_convert_YCbCr_to_RGB(y[x], cb[x], cr[x], out, out + 1, out + 2);
                out[3] = 0xff;
                out += x_inc;
            }
            y += C_WIDTH;
            cb += C_WIDTH;
            cr += C_WIDTH;
            output += y_inc;
        }
    }
    else { // bgra
        for (int v = 0; v < max_height; v++) {
            uint8_t *out = output;
            for (int x = 0; x < max_width; x++) {
                ok_jpg_convert_YCbCr_to_RGB(y[x], cb[x], cr[x], out + 2, out + 1, out);
                out[3] = 0xff;
                out += x_inc;
            }
            y += C_WIDTH;
            cb += C_WIDTH;
            cr += C_WIDTH;
            output += y_inc;
        }
    }
}