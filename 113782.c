
void nhmldmx_finalize(GF_Filter *filter)
{
	GF_NHMLDmxCtx *ctx = gf_filter_get_udta(filter);
	if (ctx->mdia) gf_fclose(ctx->mdia);
	if (ctx->parser)
		gf_xml_dom_del(ctx->parser);

#ifndef GPAC_DISABLE_ZLIB
	if (ctx->dictionary) gf_free(ctx->dictionary);
#endif
	if (ctx->bs_r) gf_bs_del(ctx->bs_r);
	if (ctx->bs_w) gf_bs_del(ctx->bs_w);
	if (ctx->samp_buffer) gf_free(ctx->samp_buffer);
	if (ctx->zlib_buffer) gf_free(ctx->zlib_buffer);