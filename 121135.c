static int decode_var_block_data(ALSDecContext *ctx, ALSBlockData *bd)
{
    ALSSpecificConfig *sconf = &ctx->sconf;
    unsigned int block_length = bd->block_length;
    unsigned int smp = 0;
    unsigned int k;
    int opt_order             = *bd->opt_order;
    int sb;
    int64_t y;
    int32_t *quant_cof        = bd->quant_cof;
    int32_t *lpc_cof          = bd->lpc_cof;
    int32_t *raw_samples      = bd->raw_samples;
    int32_t *raw_samples_end  = bd->raw_samples + bd->block_length;
    int32_t *lpc_cof_reversed = ctx->lpc_cof_reversed_buffer;

    // reverse long-term prediction
    if (*bd->use_ltp) {
        int ltp_smp;

        for (ltp_smp = FFMAX(*bd->ltp_lag - 2, 0); ltp_smp < block_length; ltp_smp++) {
            int center = ltp_smp - *bd->ltp_lag;
            int begin  = FFMAX(0, center - 2);
            int end    = center + 3;
            int tab    = 5 - (end - begin);
            int base;

            y = 1 << 6;

            for (base = begin; base < end; base++, tab++)
                y += MUL64(bd->ltp_gain[tab], raw_samples[base]);

            raw_samples[ltp_smp] += y >> 7;
        }
    }

    // reconstruct all samples from residuals
    if (bd->ra_block) {
        for (smp = 0; smp < opt_order; smp++) {
            y = 1 << 19;

            for (sb = 0; sb < smp; sb++)
                y += MUL64(lpc_cof[sb], raw_samples[-(sb + 1)]);

            *raw_samples++ -= y >> 20;
            parcor_to_lpc(smp, quant_cof, lpc_cof);
        }
    } else {
        for (k = 0; k < opt_order; k++)
            parcor_to_lpc(k, quant_cof, lpc_cof);

        // store previous samples in case that they have to be altered
        if (*bd->store_prev_samples)
            memcpy(bd->prev_raw_samples, raw_samples - sconf->max_order,
                   sizeof(*bd->prev_raw_samples) * sconf->max_order);

        // reconstruct difference signal for prediction (joint-stereo)
        if (bd->js_blocks && bd->raw_other) {
            int32_t *left, *right;

            if (bd->raw_other > raw_samples) {  // D = R - L
                left  = raw_samples;
                right = bd->raw_other;
            } else {                                // D = R - L
                left  = bd->raw_other;
                right = raw_samples;
            }

            for (sb = -1; sb >= -sconf->max_order; sb--)
                raw_samples[sb] = right[sb] - left[sb];
        }

        // reconstruct shifted signal
        if (*bd->shift_lsbs)
            for (sb = -1; sb >= -sconf->max_order; sb--)
                raw_samples[sb] >>= *bd->shift_lsbs;
    }

    // reverse linear prediction coefficients for efficiency
    lpc_cof = lpc_cof + opt_order;

    for (sb = 0; sb < opt_order; sb++)
        lpc_cof_reversed[sb] = lpc_cof[-(sb + 1)];

    // reconstruct raw samples
    raw_samples = bd->raw_samples + smp;
    lpc_cof     = lpc_cof_reversed + opt_order;

    for (; raw_samples < raw_samples_end; raw_samples++) {
        y = 1 << 19;

        for (sb = -opt_order; sb < 0; sb++)
            y += MUL64(lpc_cof[sb], raw_samples[sb]);

        *raw_samples -= y >> 20;
    }

    raw_samples = bd->raw_samples;

    // restore previous samples in case that they have been altered
    if (*bd->store_prev_samples)
        memcpy(raw_samples - sconf->max_order, bd->prev_raw_samples,
               sizeof(*raw_samples) * sconf->max_order);

    return 0;
}