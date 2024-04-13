static GF_Err xviddec_initialize(GF_Filter *filter)
{
	GF_XVIDCtx *ctx = gf_filter_get_udta(filter);
	if (!xvid_is_init) {
#ifdef XVID_USE_OLD_API
		XVID_INIT_PARAM init;
		init.api_version = 0;
		init.core_build = 0;
		/*get info*/
		init.cpu_flags = XVID_CPU_CHKONLY;
		xvid_init(NULL, 0, &init, NULL);
		/*then init*/
		xvid_init(NULL, 0, &init, NULL);
#else
		xvid_gbl_init_t init;
		init.debug = 0;
		init.version = XVID_VERSION;
		init.cpu_flags = 0; /*autodetect*/
		xvid_global(NULL, 0, &init, NULL);
#endif
		xvid_is_init = GF_TRUE;
	}

#ifndef XVID_USE_OLD_API
	if (ctx->film_effect) ctx->base_filters |= XVID_FILMEFFECT;
#endif

#ifdef XVID_USE_OLD_API
	if (ctx->deblock_y) ctx->base_filters |= XVID_DEC_DEBLOCKY;
#else
	if (ctx->deblock_y) ctx->base_filters |= XVID_DEBLOCKY;
#endif

#ifdef XVID_USE_OLD_API
	if (ctx->deblock_uv) ctx->base_filters |= XVID_DEC_DEBLOCKUV;
#else
	if (ctx->deblock_uv) ctx->base_filters |= XVID_DEBLOCKUV;
#endif

#ifndef XVID_USE_OLD_API
	if (ctx->dering_y) ctx->base_filters |= XVID_DERINGY | XVID_DEBLOCKY;
	if (ctx->dering_uv) ctx->base_filters |= XVID_DERINGUV | XVID_DEBLOCKUV;
#endif
	ctx->src_packets = gf_list_new();
	return GF_OK;
}