static GF_Err xviddec_process(GF_Filter *filter)
{
#ifdef XVID_USE_OLD_API
	XVID_DEC_FRAME frame;
#else
	xvid_dec_frame_t frame;
#endif
	u8 *buffer;
	u32 i, count;
	Bool is_seek;
#if 0
	s32 postproc;
#endif
	s32 res;
	GF_XVIDCtx *ctx = gf_filter_get_udta(filter);
	GF_FilterPacket *pck, *pck_ref, *src_pck, *dst_pck;

	pck = gf_filter_pid_get_packet(ctx->ipid);

	if (!ctx->codec)
		return ctx->cfg_crc ? GF_SERVICE_ERROR : GF_OK;

	memset(&frame, 0, sizeof(frame));
	if (pck) {
		u64 cts = gf_filter_pck_get_cts(pck);;
		frame.bitstream = (char *) gf_filter_pck_get_data(pck, &frame.length);

		//append in cts order since we get output frames in cts order
		pck_ref = pck;
		gf_filter_pck_ref_props(&pck_ref);
		count = gf_list_count(ctx->src_packets);
		src_pck = NULL;
		for (i=0; i<count; i++) {
			u64 acts;
			src_pck = gf_list_get(ctx->src_packets, i);
			acts = gf_filter_pck_get_cts(src_pck);
			if (acts==cts) {
				gf_filter_pck_unref(pck_ref);
				break;
			}
			if (acts>cts) {
				gf_list_insert(ctx->src_packets, pck_ref, i);
				break;
			}
			src_pck = NULL;
		}
		if (!src_pck)
			gf_list_add(ctx->src_packets, pck_ref);


	} else {
		frame.bitstream = NULL;
		frame.length = -1;
	}

packed_frame :

	dst_pck = gf_filter_pck_new_alloc(ctx->opid, ctx->width*ctx->height*3/2, &buffer);
	if (!dst_pck) return GF_OUT_OF_MEM;

#ifdef XVID_USE_OLD_API
	frame.colorspace = XVID_CSP_I420;
	frame.stride = ctx->width;
	frame.image = (void *) buffer;
#else
	frame.version = XVID_VERSION;
	frame.output.csp = XVID_CSP_I420;
	frame.output.stride[0] = ctx->width;
	frame.output.plane[0] = (void *) buffer;
#endif


#if 0
	postproc = ctx->base_filters;
	/*to check, not convinced yet by results...*/
	switch (mmlevel) {
	case GF_CODEC_LEVEL_SEEK:
	case GF_CODEC_LEVEL_DROP:
		/*turn off all post-proc*/
#ifdef XVID_USE_OLD_API
		postproc &= ~XVID_DEC_DEBLOCKY;
		postproc &= ~XVID_DEC_DEBLOCKUV;
#else
		postproc &= ~XVID_DEBLOCKY;
		postproc &= ~XVID_DEBLOCKUV;
		postproc &= ~XVID_FILMEFFECT;
#endif
		break;
	case GF_CODEC_LEVEL_VERY_LATE:
		/*turn off post-proc*/
#ifdef XVID_USE_OLD_API
		postproc &= ~XVID_DEC_DEBLOCKY;
#else
		postproc &= ~XVID_FILMEFFECT;
		postproc &= ~XVID_DEBLOCKY;
#endif
		break;
	case GF_CODEC_LEVEL_LATE:
#ifdef XVID_USE_OLD_API
		postproc &= ~XVID_DEC_DEBLOCKUV;
#else
		postproc &= ~XVID_DEBLOCKUV;
		postproc &= ~XVID_FILMEFFECT;
#endif
		break;
	}
#endif

	/*xvid may keep the first I frame and force a 1-frame delay, so we simply trick it*/
	if (ctx->first_frame) {
		buffer[0] = 'v';
		buffer[1] = 'o';
		buffer[2] = 'i';
		buffer[3] = 'd';
	}
	src_pck = gf_list_get(ctx->src_packets, 0);

	res = xvid_decore(ctx->codec, XVID_DEC_DECODE, &frame, NULL);
	if (res < 0) {
		gf_filter_pck_discard(dst_pck);
		if (pck) gf_filter_pid_drop_packet(ctx->ipid);
		if (src_pck) {
			gf_filter_pck_unref(src_pck);
			gf_list_pop_front(ctx->src_packets);
		}
		if (gf_filter_pid_is_eos(ctx->ipid)) {
			gf_filter_pid_set_eos(ctx->opid);
			return GF_EOS;
		}
		return pck ? GF_NON_COMPLIANT_BITSTREAM : GF_OK;
	}

	if (ctx->first_frame) {
		ctx->first_frame = GF_FALSE;
		if ((buffer[0] == 'v') && (buffer[1] == 'o') && (buffer[2] == 'i') && (buffer[3] =='d')) {
			gf_filter_pck_discard(dst_pck);
			if (pck) gf_filter_pid_drop_packet(ctx->ipid);
			return GF_OK;
		}
	}

	if (src_pck) {
		gf_filter_pck_merge_properties(src_pck, dst_pck);
		is_seek = gf_filter_pck_get_seek_flag(src_pck);
		ctx->next_cts = gf_filter_pck_get_cts(src_pck);
		gf_filter_pck_set_dts(dst_pck, ctx->next_cts);
		ctx->next_cts += gf_filter_pck_get_duration(src_pck);
		gf_filter_pck_unref(src_pck);
		gf_list_pop_front(ctx->src_packets);
	} else {
		is_seek = 0;
		gf_filter_pck_set_cts(dst_pck, ctx->next_cts);
	}

	if (!pck || !is_seek )
		gf_filter_pck_send(dst_pck);
	else
		gf_filter_pck_discard(dst_pck);

	if (res + 6 < frame.length) {
		frame.bitstream = ((char *)frame.bitstream) + res;
		frame.length -= res;
		goto packed_frame;
	}

	if (pck) {
		gf_filter_pid_drop_packet(ctx->ipid);
	}
	//flush all frames if eos is detected
	else if (gf_filter_pid_is_eos(ctx->ipid)) {
		return xviddec_process(filter);
	}

	return GF_OK;
}