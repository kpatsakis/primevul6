static void adts_dmx_finalize(GF_Filter *filter)
{
	GF_ADTSDmxCtx *ctx = gf_filter_get_udta(filter);
	if (ctx->bs) gf_bs_del(ctx->bs);
	if (ctx->indexes) gf_free(ctx->indexes);
	if (ctx->adts_buffer) gf_free(ctx->adts_buffer);
	if (ctx->id3_buffer) gf_free(ctx->id3_buffer);
	if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
}