static void ok_jpg_progressive_finish(ok_jpg_decoder *decoder) {
    int16_t out_block[64];
    for (int i = 0; i < decoder->num_components; i++) {
        ok_jpg_component *c = decoder->components + i;
        c->next_block = 0;
    }
    for (int data_unit_y = 0; data_unit_y < decoder->data_units_y; data_unit_y++) {
        for (int data_unit_x = 0; data_unit_x < decoder->data_units_x; data_unit_x++) {
            for (int i = 0; i < decoder->num_components; i++) {
                ok_jpg_component *c = decoder->components + i;
                size_t block_index = c->next_block;
                int offset_y = 0;
                for (int y = 0; y < c->V; y++) {
                    int offset_x = 0;
                    for (int x = 0; x < c->H; x++) {
                        int16_t *in_block = c->blocks + (block_index * 64);
                        ok_jpg_dequantize(decoder, c, in_block, out_block);
                        c->idct(out_block, c->output + offset_x + offset_y);
                        block_index++;
                        offset_x += 8;
                    }
                    offset_y += C_WIDTH * 8;
                    block_index += (size_t)(c->H * (decoder->data_units_x - 1));
                }
                c->next_block += c->H;
            }

            ok_jpg_convert_data_unit(decoder, data_unit_x, data_unit_y);
        }
        for (int i = 0; i < decoder->num_components; i++) {
            ok_jpg_component *c = decoder->components + i;
            c->next_block += (size_t)((c->V - 1) * c->H * decoder->data_units_x);
        }
    }
}