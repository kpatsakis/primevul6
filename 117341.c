static av_cold int dnxhd_init_qmat(DNXHDEncContext *ctx, int lbias, int cbias)
{
    // init first elem to 1 to avoid div by 0 in convert_matrix
    uint16_t weight_matrix[64] = {1,}; // convert_matrix needs uint16_t*
    int qscale, i;
    const uint8_t *luma_weight_table   = ctx->cid_table->luma_weight;
    const uint8_t *chroma_weight_table = ctx->cid_table->chroma_weight;

    FF_ALLOCZ_OR_GOTO(ctx->m.avctx, ctx->qmatrix_l,   (ctx->m.avctx->qmax+1) * 64 *     sizeof(int),      fail);
    FF_ALLOCZ_OR_GOTO(ctx->m.avctx, ctx->qmatrix_c,   (ctx->m.avctx->qmax+1) * 64 *     sizeof(int),      fail);
    FF_ALLOCZ_OR_GOTO(ctx->m.avctx, ctx->qmatrix_l16, (ctx->m.avctx->qmax+1) * 64 * 2 * sizeof(uint16_t), fail);
    FF_ALLOCZ_OR_GOTO(ctx->m.avctx, ctx->qmatrix_c16, (ctx->m.avctx->qmax+1) * 64 * 2 * sizeof(uint16_t), fail);

    if (ctx->cid_table->bit_depth == 8) {
        for (i = 1; i < 64; i++) {
            int j = ctx->m.dsp.idct_permutation[ff_zigzag_direct[i]];
            weight_matrix[j] = ctx->cid_table->luma_weight[i];
        }
        ff_convert_matrix(&ctx->m.dsp, ctx->qmatrix_l, ctx->qmatrix_l16, weight_matrix,
                          ctx->m.intra_quant_bias, 1, ctx->m.avctx->qmax, 1);
        for (i = 1; i < 64; i++) {
            int j = ctx->m.dsp.idct_permutation[ff_zigzag_direct[i]];
            weight_matrix[j] = ctx->cid_table->chroma_weight[i];
        }
        ff_convert_matrix(&ctx->m.dsp, ctx->qmatrix_c, ctx->qmatrix_c16, weight_matrix,
                          ctx->m.intra_quant_bias, 1, ctx->m.avctx->qmax, 1);

        for (qscale = 1; qscale <= ctx->m.avctx->qmax; qscale++) {
            for (i = 0; i < 64; i++) {
                ctx->qmatrix_l  [qscale]   [i] <<= 2; ctx->qmatrix_c  [qscale]   [i] <<= 2;
                ctx->qmatrix_l16[qscale][0][i] <<= 2; ctx->qmatrix_l16[qscale][1][i] <<= 2;
                ctx->qmatrix_c16[qscale][0][i] <<= 2; ctx->qmatrix_c16[qscale][1][i] <<= 2;
            }
        }
    } else {
        // 10-bit
        for (qscale = 1; qscale <= ctx->m.avctx->qmax; qscale++) {
            for (i = 1; i < 64; i++) {
                int j = ctx->m.dsp.idct_permutation[ff_zigzag_direct[i]];

                // The quantization formula from the VC-3 standard is:
                // quantized = sign(block[i]) * floor(abs(block[i]/s) * p / (qscale * weight_table[i]))
                // Where p is 32 for 8-bit samples and 8 for 10-bit ones.
                // The s factor compensates scaling of DCT coefficients done by the DCT routines,
                // and therefore is not present in standard.  It's 8 for 8-bit samples and 4 for 10-bit ones.
                // We want values of ctx->qtmatrix_l and ctx->qtmatrix_r to be:
                // ((1 << DNX10BIT_QMAT_SHIFT) * (p / s)) / (qscale * weight_table[i])
                // For 10-bit samples, p / s == 2
                ctx->qmatrix_l[qscale][j] = (1 << (DNX10BIT_QMAT_SHIFT + 1)) / (qscale * luma_weight_table[i]);
                ctx->qmatrix_c[qscale][j] = (1 << (DNX10BIT_QMAT_SHIFT + 1)) / (qscale * chroma_weight_table[i]);
            }
        }
    }

    ctx->m.q_chroma_intra_matrix16 = ctx->qmatrix_c16;
    ctx->m.q_chroma_intra_matrix   = ctx->qmatrix_c;
    ctx->m.q_intra_matrix16        = ctx->qmatrix_l16;
    ctx->m.q_intra_matrix          = ctx->qmatrix_l;

    return 0;
 fail:
    return -1;
}