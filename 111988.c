void naludmx_create_avc_decoder_config(GF_NALUDmxCtx *ctx, u8 **dsi, u32 *dsi_size, u8 **dsi_enh, u32 *dsi_enh_size, u32 *max_width, u32 *max_height, u32 *max_enh_width, u32 *max_enh_height, GF_Fraction *sar)
{
	u32 i, count;
	Bool first = GF_TRUE;
	Bool first_svc = GF_TRUE;
	GF_AVCConfig *cfg;
	GF_AVCConfig *avcc;
	GF_AVCConfig *svcc;
	u32 max_w, max_h, max_ew, max_eh;


	max_w = max_h = max_ew = max_eh = 0;
	sar->num = sar->den = 0;

	avcc = gf_odf_avc_cfg_new();
	svcc = gf_odf_avc_cfg_new();
	avcc->nal_unit_size = ctx->nal_length;
	svcc->nal_unit_size = ctx->nal_length;

	ctx->is_mvc = GF_FALSE;
	count = gf_list_count(ctx->sps);
	for (i=0; i<count; i++) {
		Bool is_svc = GF_FALSE;
		GF_NALUFFParam *sl = gf_list_get(ctx->sps, i);
		AVC_SPS *sps = &ctx->avc_state->sps[sl->id];
		u32 nal_type = sl->data[0] & 0x1F;

		if ((sps->profile_idc == 118) || (sps->profile_idc == 128)) {
			ctx->is_mvc = GF_TRUE;
		}

		if (ctx->explicit) {
			cfg = svcc;
		} else if (nal_type == GF_AVC_NALU_SVC_SUBSEQ_PARAM) {
			cfg = svcc;
			is_svc = GF_TRUE;
		} else {
			cfg = avcc;
		}

		if (first || (is_svc && first_svc) ) {
			cfg->configurationVersion = 1;
			cfg->profile_compatibility = sps->prof_compat;
			cfg->AVCProfileIndication = sps->profile_idc;
			cfg->AVCLevelIndication = sps->level_idc;
			cfg->chroma_format = sps->chroma_format;
			cfg->luma_bit_depth = 8 + sps->luma_bit_depth_m8;
			cfg->chroma_bit_depth = 8 + sps->chroma_bit_depth_m8;
			/*try to patch ?*/
			if (!gf_avc_is_rext_profile(cfg->AVCProfileIndication)
				&& ((cfg->chroma_format>1) || (cfg->luma_bit_depth>8) || (cfg->chroma_bit_depth>8))
			) {
				if ((cfg->luma_bit_depth>8) || (cfg->chroma_bit_depth>8)) {
					cfg->AVCProfileIndication = 110;
				} else {
					cfg->AVCProfileIndication = (cfg->chroma_format==3) ? 244 : 122;
				}
			}
			if (sps->vui_parameters_present_flag && sps->vui.par_num && sps->vui.par_den) {
				sar->num = sps->vui.par_num;
				sar->den = sps->vui.par_den;
			}
			ctx->interlaced = sps->frame_mbs_only_flag ? GF_FALSE : GF_TRUE;


			/*disable frame rate scan, most bitstreams have wrong values there*/
			if (first && (!ctx->fps.num || !ctx->fps.den) && sps->vui.timing_info_present_flag
				/*if detected FPS is greater than 1000, assume wrong timing info*/
				&& (sps->vui.time_scale <= 1000*sps->vui.num_units_in_tick)
			) {
				/*ISO/IEC 14496-10 n11084 Table E-6*/
				/* not used :				u8 DeltaTfiDivisorTable[] = {1,1,1,2,2,2,2,3,3,4,6}; */
				u8 DeltaTfiDivisorIdx;
				if (!sps->vui.pic_struct_present_flag) {
					DeltaTfiDivisorIdx = 1 + (1 - ctx->avc_state->s_info.field_pic_flag);
				} else {
					if (!ctx->avc_state->sei.pic_timing.pic_struct)
						DeltaTfiDivisorIdx = 2;
					else if (ctx->avc_state->sei.pic_timing.pic_struct == 8)
						DeltaTfiDivisorIdx = 6;
					else
						DeltaTfiDivisorIdx = (ctx->avc_state->sei.pic_timing.pic_struct+1) / 2;
				}
				if (!ctx->timescale && sps->vui.time_scale && sps->vui.num_units_in_tick) {
					ctx->cur_fps.num = 2 * sps->vui.time_scale;
					ctx->cur_fps.den = 2 * sps->vui.num_units_in_tick * DeltaTfiDivisorIdx;

					if (!ctx->fps.num && ctx->dts==ctx->fps.den)
						ctx->dts = ctx->cur_fps.den;
				}
				if (! sps->vui.fixed_frame_rate_flag)
					GF_LOG(GF_LOG_INFO, GF_LOG_PARSER, ("[%s] Possible Variable Frame Rate: VUI \"fixed_frame_rate_flag\" absent\n", ctx->log_name));
			}
			ctx->fps = ctx->cur_fps;
		}
		first = GF_FALSE;
		if (is_svc) {
			first_svc = GF_FALSE;
			if (sps->width > max_ew) max_ew = sps->width;
			if (sps->height > max_eh) max_eh = sps->height;
		} else {
			if (sps->width > max_w) max_w = sps->width;
			if (sps->height > max_h) max_h = sps->height;
		}
		if (!ctx->analyze)
			gf_list_add(cfg->sequenceParameterSets, sl);
	}

	cfg = ctx->explicit ? svcc : avcc;
	count = gf_list_count(ctx->sps_ext);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->sps_ext, i);
		if (!cfg->sequenceParameterSetExtensions) cfg->sequenceParameterSetExtensions = gf_list_new();
		if (!ctx->analyze)
			gf_list_add(cfg->sequenceParameterSetExtensions, sl);
	}

	cfg = ctx->explicit ? svcc : avcc;
	count = gf_list_count(ctx->pps);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->pps, i);
		if (!ctx->analyze)
			gf_list_add(cfg->pictureParameterSets, sl);
	}

	cfg = svcc;
	count = gf_list_count(ctx->pps_svc);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = gf_list_get(ctx->pps_svc, i);
		if (!ctx->analyze)
			gf_list_add(cfg->pictureParameterSets, sl);
	}

	*dsi = *dsi_enh = NULL;
	*dsi_size = *dsi_enh_size = 0;

	if (ctx->explicit) {
		gf_odf_avc_cfg_write(svcc, dsi, dsi_size);
	} else {
		gf_odf_avc_cfg_write(avcc, dsi, dsi_size);
		if (gf_list_count(svcc->sequenceParameterSets) || svcc->sequenceParameterSetExtensions) {
			gf_odf_avc_cfg_write(svcc, dsi_enh, dsi_enh_size);
		}
	}
	gf_list_reset(avcc->sequenceParameterSets);
	gf_list_reset(avcc->sequenceParameterSetExtensions);
	gf_list_reset(avcc->pictureParameterSets);
	gf_list_reset(svcc->sequenceParameterSets);
	gf_list_reset(svcc->sequenceParameterSetExtensions);
	gf_list_reset(svcc->pictureParameterSets);
	gf_odf_avc_cfg_del(avcc);
	gf_odf_avc_cfg_del(svcc);
	*max_width = max_w;
	*max_height = max_h;
	*max_enh_width = max_ew;
	*max_enh_height = max_eh;
}