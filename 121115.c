static int read_frame_data(ALSDecContext *ctx, unsigned int ra_frame)
{
    ALSSpecificConfig *sconf = &ctx->sconf;
    AVCodecContext *avctx    = ctx->avctx;
    GetBitContext *gb = &ctx->gb;
    unsigned int div_blocks[32];                ///< block sizes.
    unsigned int c;
    unsigned int js_blocks[2];
    uint32_t bs_info = 0;
    int ret;

    // skip the size of the ra unit if present in the frame
    if (sconf->ra_flag == RA_FLAG_FRAMES && ra_frame)
        skip_bits_long(gb, 32);

    if (sconf->mc_coding && sconf->joint_stereo) {
        ctx->js_switch = get_bits1(gb);
        align_get_bits(gb);
    }

    if (!sconf->mc_coding || ctx->js_switch) {
        int independent_bs = !sconf->joint_stereo;

        for (c = 0; c < avctx->channels; c++) {
            js_blocks[0] = 0;
            js_blocks[1] = 0;

            get_block_sizes(ctx, div_blocks, &bs_info);

            // if joint_stereo and block_switching is set, independent decoding
            // is signaled via the first bit of bs_info
            if (sconf->joint_stereo && sconf->block_switching)
                if (bs_info >> 31)
                    independent_bs = 2;

            // if this is the last channel, it has to be decoded independently
            if (c == avctx->channels - 1)
                independent_bs = 1;

            if (independent_bs) {
                ret = decode_blocks_ind(ctx, ra_frame, c,
                                        div_blocks, js_blocks);
                if (ret < 0)
                    return ret;
                independent_bs--;
            } else {
                ret = decode_blocks(ctx, ra_frame, c, div_blocks, js_blocks);
                if (ret < 0)
                    return ret;

                c++;
            }

            // store carryover raw samples
            memmove(ctx->raw_samples[c] - sconf->max_order,
                    ctx->raw_samples[c] - sconf->max_order + sconf->frame_length,
                    sizeof(*ctx->raw_samples[c]) * sconf->max_order);
        }
    } else { // multi-channel coding
        ALSBlockData   bd = { 0 };
        int            b, ret;
        int            *reverted_channels = ctx->reverted_channels;
        unsigned int   offset             = 0;

        for (c = 0; c < avctx->channels; c++)
            if (ctx->chan_data[c] < ctx->chan_data_buffer) {
                av_log(ctx->avctx, AV_LOG_ERROR, "Invalid channel data.\n");
                return AVERROR_INVALIDDATA;
            }

        memset(reverted_channels, 0, sizeof(*reverted_channels) * avctx->channels);

        bd.ra_block         = ra_frame;
        bd.prev_raw_samples = ctx->prev_raw_samples;

        get_block_sizes(ctx, div_blocks, &bs_info);

        for (b = 0; b < ctx->num_blocks; b++) {
            bd.block_length = div_blocks[b];
            if (bd.block_length <= 0) {
                av_log(ctx->avctx, AV_LOG_WARNING,
                       "Invalid block length %d in channel data!\n", bd.block_length);
                continue;
            }

            for (c = 0; c < avctx->channels; c++) {
                bd.const_block = ctx->const_block + c;
                bd.shift_lsbs  = ctx->shift_lsbs + c;
                bd.opt_order   = ctx->opt_order + c;
                bd.store_prev_samples = ctx->store_prev_samples + c;
                bd.use_ltp     = ctx->use_ltp + c;
                bd.ltp_lag     = ctx->ltp_lag + c;
                bd.ltp_gain    = ctx->ltp_gain[c];
                bd.lpc_cof     = ctx->lpc_cof[c];
                bd.quant_cof   = ctx->quant_cof[c];
                bd.raw_samples = ctx->raw_samples[c] + offset;
                bd.raw_other   = NULL;

                if ((ret = read_block(ctx, &bd)) < 0)
                    return ret;
                if ((ret = read_channel_data(ctx, ctx->chan_data[c], c)) < 0)
                    return ret;
            }

            for (c = 0; c < avctx->channels; c++) {
                ret = revert_channel_correlation(ctx, &bd, ctx->chan_data,
                                                 reverted_channels, offset, c);
                if (ret < 0)
                    return ret;
            }
            for (c = 0; c < avctx->channels; c++) {
                bd.const_block = ctx->const_block + c;
                bd.shift_lsbs  = ctx->shift_lsbs + c;
                bd.opt_order   = ctx->opt_order + c;
                bd.store_prev_samples = ctx->store_prev_samples + c;
                bd.use_ltp     = ctx->use_ltp + c;
                bd.ltp_lag     = ctx->ltp_lag + c;
                bd.ltp_gain    = ctx->ltp_gain[c];
                bd.lpc_cof     = ctx->lpc_cof[c];
                bd.quant_cof   = ctx->quant_cof[c];
                bd.raw_samples = ctx->raw_samples[c] + offset;

                if ((ret = decode_block(ctx, &bd)) < 0)
                    return ret;
            }

            memset(reverted_channels, 0, avctx->channels * sizeof(*reverted_channels));
            offset      += div_blocks[b];
            bd.ra_block  = 0;
        }

        // store carryover raw samples
        for (c = 0; c < avctx->channels; c++)
            memmove(ctx->raw_samples[c] - sconf->max_order,
                    ctx->raw_samples[c] - sconf->max_order + sconf->frame_length,
                    sizeof(*ctx->raw_samples[c]) * sconf->max_order);
    }

    // TODO: read_diff_float_data

    return 0;
}