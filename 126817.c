static void decode_rgb_frame(FFV1Context *s, uint8_t *src[3], int w, int h, int stride[3])
{
    int x, y, p;
    int16_t *sample[4][2];
    int lbd    = s->avctx->bits_per_raw_sample <= 8;
    int bits   = s->avctx->bits_per_raw_sample > 0 ? s->avctx->bits_per_raw_sample : 8;
    int offset = 1 << bits;

    for (x = 0; x < 4; x++) {
        sample[x][0] = s->sample_buffer +  x * 2      * (w + 6) + 3;
        sample[x][1] = s->sample_buffer + (x * 2 + 1) * (w + 6) + 3;
    }

    s->run_index = 0;

    memset(s->sample_buffer, 0, 8 * (w + 6) * sizeof(*s->sample_buffer));

    for (y = 0; y < h; y++) {
        for (p = 0; p < 3 + s->transparency; p++) {
            int16_t *temp = sample[p][0]; // FIXME: try a normal buffer

            sample[p][0] = sample[p][1];
            sample[p][1] = temp;

            sample[p][1][-1]= sample[p][0][0  ];
            sample[p][0][ w]= sample[p][0][w-1];
            if (lbd && s->slice_coding_mode == 0)
                decode_line(s, w, sample[p], (p + 1)/2, 9);
            else
                decode_line(s, w, sample[p], (p + 1)/2, bits + (s->slice_coding_mode != 1));
        }
        for (x = 0; x < w; x++) {
            int g = sample[0][1][x];
            int b = sample[1][1][x];
            int r = sample[2][1][x];
            int a = sample[3][1][x];

            if (s->slice_coding_mode != 1) {
                b -= offset;
                r -= offset;
                g -= (b * s->slice_rct_by_coef + r * s->slice_rct_ry_coef) >> 2;
                b += g;
                r += g;
            }

            if (lbd)
                *((uint32_t*)(src[0] + x*4 + stride[0]*y)) = b + (g<<8) + (r<<16) + (a<<24);
            else {
                *((uint16_t*)(src[0] + x*2 + stride[0]*y)) = b;
                *((uint16_t*)(src[1] + x*2 + stride[1]*y)) = g;
                *((uint16_t*)(src[2] + x*2 + stride[2]*y)) = r;
            }
        }
    }
}