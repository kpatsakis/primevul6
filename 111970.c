static void naludmx_check_dur(GF_Filter *filter, GF_NALUDmxCtx *ctx)
{
	FILE *stream;
	GF_BitStream *bs;
	u64 duration, cur_dur, nal_start, start_code_pos, rate;
	AVCState *avc_state = NULL;
	HEVCState *hevc_state = NULL;
	VVCState *vvc_state = NULL;
	Bool first_slice_in_pic = GF_TRUE;
	const GF_PropertyValue *p;
	const char *filepath = NULL;
	if (!ctx->opid || ctx->timescale || ctx->file_loaded) return;

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILEPATH);
	if (!p || !p->value.string || !strncmp(p->value.string, "gmem://", 7)) {
		ctx->is_file = GF_FALSE;
		ctx->file_loaded = GF_TRUE;
		return;
	}
	filepath = p->value.string;
	ctx->is_file = GF_TRUE;

	if (ctx->index<0) {
		if (gf_opts_get_bool("temp", "force_indexing")) {
			ctx->index = 1.0;
		} else {
			p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_DOWN_SIZE);
			if (!p || (p->value.longuint > 20000000)) {
				GF_LOG(GF_LOG_INFO, GF_LOG_PARSER, ("[%s] Source file larger than 20M, skipping indexing\n", ctx->log_name));
			} else {
				ctx->index = -ctx->index;
			}
		}
	}
	if (ctx->index<=0) {
		ctx->duration.num = 1;
		ctx->file_loaded = GF_TRUE;
		return;
	}

	if (ctx->codecid==GF_CODECID_HEVC) {
		GF_SAFEALLOC(hevc_state, HEVCState);
		if (!hevc_state) return;
	} else if (ctx->codecid==GF_CODECID_VVC) {
		GF_SAFEALLOC(vvc_state, VVCState);
		if (!vvc_state) return;
	} else {
		GF_SAFEALLOC(avc_state, AVCState);
		if (!avc_state) return;
	}

	stream = gf_fopen(filepath, "rb");
	if (!stream) {
		if (hevc_state) gf_free(hevc_state);
		if (vvc_state) gf_free(vvc_state);
		if (avc_state) gf_free(avc_state);
		return;
	}
	ctx->index_size = 0;
	duration = 0;
	cur_dur = 0;

	bs = gf_bs_from_file(stream, GF_BITSTREAM_READ);
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);

	start_code_pos = gf_bs_get_position(bs);
	if (!gf_media_nalu_is_start_code(bs)) {
		if (hevc_state) gf_free(hevc_state);
		if (avc_state) gf_free(avc_state);
		gf_bs_del(bs);
		gf_fclose(stream);
		ctx->duration.num = 1;
		ctx->file_loaded = GF_TRUE;
		return;
	}

	nal_start = gf_bs_get_position(bs);

	while (gf_bs_available(bs)) {
		u32 nal_size;
		s32 res;
		Bool is_rap = GF_FALSE;
		Bool is_slice = GF_FALSE;
		nal_size = gf_media_nalu_next_start_code_bs(bs);

		gf_bs_seek(bs, nal_start);
		if (hevc_state) {
#ifndef GPAC_DISABLE_HEVC
			u8 temporal_id, layer_id, nal_type;

			res = gf_hevc_parse_nalu_bs(bs, hevc_state, &nal_type, &temporal_id, &layer_id);
			if (res>0) first_slice_in_pic = GF_TRUE;
			switch (nal_type) {
			case GF_HEVC_NALU_SLICE_IDR_N_LP:
			case GF_HEVC_NALU_SLICE_IDR_W_DLP:
			case GF_HEVC_NALU_SLICE_CRA:
			case GF_HEVC_NALU_SLICE_BLA_N_LP:
			case GF_HEVC_NALU_SLICE_BLA_W_LP:
			case GF_HEVC_NALU_SLICE_BLA_W_DLP:
				is_rap = GF_TRUE;
				is_slice = GF_TRUE;
				break;
			case GF_HEVC_NALU_SLICE_STSA_N:
			case GF_HEVC_NALU_SLICE_STSA_R:
			case GF_HEVC_NALU_SLICE_RADL_R:
			case GF_HEVC_NALU_SLICE_RASL_R:
			case GF_HEVC_NALU_SLICE_RADL_N:
			case GF_HEVC_NALU_SLICE_RASL_N:
			case GF_HEVC_NALU_SLICE_TRAIL_N:
			case GF_HEVC_NALU_SLICE_TRAIL_R:
			case GF_HEVC_NALU_SLICE_TSA_N:
			case GF_HEVC_NALU_SLICE_TSA_R:
				is_slice = GF_TRUE;
				break;
			}
#endif // GPAC_DISABLE_HEVC
		} else if (vvc_state) {

		} else {
			u32 nal_type;
			u64 pos = gf_bs_get_position(bs);
			res = gf_avc_parse_nalu(bs, avc_state);
			if (res>0) first_slice_in_pic = GF_TRUE;

			nal_type = avc_state->last_nal_type_parsed;

			switch (nal_type) {
			case GF_AVC_NALU_SEQ_PARAM:
				gf_bs_seek(bs, pos);
				gf_avc_read_sps_bs(bs, avc_state, GF_FALSE, NULL);
				break;
			case GF_AVC_NALU_PIC_PARAM:
				gf_bs_seek(bs, pos);
				gf_avc_read_pps_bs(bs, avc_state);
				break;
			case GF_AVC_NALU_IDR_SLICE:
				is_rap = GF_TRUE;
				is_slice = GF_TRUE;
				break;
			case GF_AVC_NALU_NON_IDR_SLICE:
			case GF_AVC_NALU_DP_A_SLICE:
			case GF_AVC_NALU_DP_B_SLICE:
			case GF_AVC_NALU_DP_C_SLICE:
				is_slice = GF_TRUE;
				break;
			}
		}

		if (is_rap && first_slice_in_pic && (cur_dur >= ctx->index * ctx->cur_fps.num) ) {
			if (!ctx->index_alloc_size) ctx->index_alloc_size = 10;
			else if (ctx->index_alloc_size == ctx->index_size) ctx->index_alloc_size *= 2;
			ctx->indexes = gf_realloc(ctx->indexes, sizeof(NALUIdx)*ctx->index_alloc_size);
			ctx->indexes[ctx->index_size].pos = start_code_pos;
			ctx->indexes[ctx->index_size].duration = (Double) duration;
			ctx->indexes[ctx->index_size].duration /= ctx->cur_fps.num;
			ctx->index_size ++;
			cur_dur = 0;
		}

		if (is_slice && first_slice_in_pic) {
			duration += ctx->cur_fps.den;
			cur_dur += ctx->cur_fps.den;
			first_slice_in_pic = GF_FALSE;
		}

		gf_bs_seek(bs, nal_start + nal_size);
/*		nal_start = gf_media_nalu_next_start_code_bs(bs);
		if (nal_start) gf_bs_skip_bytes(bs, nal_start);
*/
		if (gf_bs_available(bs)<4)
			break;

		start_code_pos = gf_bs_get_position(bs);
		nal_start = gf_media_nalu_is_start_code(bs);
		if (!nal_start) {
			break;
		}
		nal_start = gf_bs_get_position(bs);
	}

	rate = gf_bs_get_position(bs);
	gf_bs_del(bs);
	gf_fclose(stream);
	if (hevc_state) gf_free(hevc_state);
	if (vvc_state) gf_free(vvc_state);
	if (avc_state) gf_free(avc_state);

	if (!ctx->duration.num || (ctx->duration.num  * ctx->cur_fps.num != duration * ctx->duration.den)) {
		ctx->duration.num = (s32) duration;
		ctx->duration.den = ctx->cur_fps.num;

		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, & PROP_FRAC64(ctx->duration));

		if (duration && (!gf_sys_is_test_mode() || gf_opts_get_bool("temp", "force_indexing"))) {
			rate *= 8 * ctx->duration.den;
			rate /= ctx->duration.num;
			ctx->bitrate = (u32) rate;
		}
	}


	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILE_CACHED);
	if (p && p->value.boolean) ctx->file_loaded = GF_TRUE;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CAN_DATAREF, & PROP_BOOL(GF_TRUE ) );
}