static void ok_jpg_convert_data_unit(ok_jpg_decoder *decoder, int data_unit_x, int data_unit_y) {
    ok_jpg *jpg = decoder->jpg;
    ok_jpg_component *c = decoder->components;
    int x = data_unit_x * c->H * 8;
    int y = data_unit_y * c->V * 8;
    const int width = min(c->H * 8, decoder->in_width - x);
    const int height = min(c->V * 8, decoder->in_height - y);
    int x_inc = 4;
    int y_inc = (int)jpg->stride;
    uint8_t *data = jpg->data;
    if (decoder->rotate) {
        int temp = x;
        x = y;
        y = temp;
    }
    if (decoder->flip_x) {
        data += (jpg->width * 4) - ((size_t)x + 1) * (size_t)x_inc;
        x_inc = -x_inc;
    } else {
        data += (size_t)x * (size_t)x_inc;
    }
    if (decoder->flip_y) {
        data += ((jpg->height - (size_t)y - 1) * (size_t)y_inc);
        y_inc = -y_inc;
    } else {
        data += (size_t)y * (size_t)y_inc;
    }
    if (decoder->rotate) {
        int temp = x_inc;
        x_inc = y_inc;
        y_inc = temp;
    }

    if (decoder->num_components == 1) {
        ok_jpg_convert_data_unit_grayscale(c->output, data, x_inc, y_inc, width, height);
    } else {
        ok_jpg_convert_data_unit_color(c->output, (c + 1)->output, (c + 2)->output,
                                       data, decoder->color_rgba,
                                       x_inc, y_inc, width, height);
    }
}