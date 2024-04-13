static int dnxhd_decode_row(AVCodecContext *avctx, void *data,
                            int rownb, int threadnb)
{
    const DNXHDContext *ctx = avctx->priv_data;
    uint32_t offset = ctx->mb_scan_index[rownb];
    RowContext *row = ctx->rows + threadnb;
    int x, ret;

    row->last_dc[0] =
    row->last_dc[1] =
    row->last_dc[2] = 1 << (ctx->bit_depth + 2); // for levels +2^(bitdepth-1)
    ret = init_get_bits8(&row->gb, ctx->buf + offset, ctx->buf_size - offset);
    if (ret < 0) {
        row->errors++;
        return ret;
    }
    for (x = 0; x < ctx->mb_width; x++) {
        int ret = dnxhd_decode_macroblock(ctx, row, data, x, rownb);
        if (ret < 0) {
            row->errors++;
            return ret;
        }
    }

    return 0;
}