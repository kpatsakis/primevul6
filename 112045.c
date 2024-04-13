static void xviddec_finalize(GF_Filter *filter)
{
	GF_XVIDCtx *ctx = gf_filter_get_udta(filter);
	if (ctx->codec) xvid_decore(ctx->codec, XVID_DEC_DESTROY, NULL, NULL);
	while (gf_list_count(ctx->src_packets)) {
		GF_FilterPacket *pck = gf_list_pop_back(ctx->src_packets);
		gf_filter_pck_unref(pck);
	}
	gf_list_del(ctx->src_packets);
}