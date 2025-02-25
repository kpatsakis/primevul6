static Bool hevc_parse_short_term_ref_pic_set(GF_BitStream *bs, HEVC_SPS *sps, u32 idx_rps)
{
	u32 i;
	Bool inter_ref_pic_set_prediction_flag = 0;
	if (idx_rps != 0)
		inter_ref_pic_set_prediction_flag = gf_bs_read_int_log_idx(bs, 1, "inter_ref_pic_set_prediction_flag", idx_rps);

	if (inter_ref_pic_set_prediction_flag) {
		HEVC_ReferencePictureSets *ref_ps, *rps;
		u32 delta_idx_minus1 = 0;
		u32 ref_idx;
		u32 delta_rps_sign;
		u32 abs_delta_rps_minus1, nb_ref_pics;
		s32 deltaRPS;
		u32 k = 0, k0 = 0, k1 = 0;
		if (idx_rps == sps->num_short_term_ref_pic_sets)
			delta_idx_minus1 = gf_bs_read_ue_log_idx(bs, "delta_idx_minus1", idx_rps);

		assert(delta_idx_minus1 <= idx_rps - 1);
		ref_idx = idx_rps - 1 - delta_idx_minus1;
		delta_rps_sign = gf_bs_read_int_log_idx(bs, 1, "delta_rps_sign", idx_rps);
		abs_delta_rps_minus1 = gf_bs_read_ue_log_idx(bs, "abs_delta_rps_minus1", idx_rps);
		deltaRPS = (1 - (delta_rps_sign << 1)) * (abs_delta_rps_minus1 + 1);

		rps = &sps->rps[idx_rps];
		ref_ps = &sps->rps[ref_idx];
		nb_ref_pics = ref_ps->num_negative_pics + ref_ps->num_positive_pics;
		for (i = 0; i <= nb_ref_pics; i++) {
			s32 ref_idc;
			s32 used_by_curr_pic_flag = gf_bs_read_int_log_idx2(bs, 1, "used_by_curr_pic_flag", idx_rps, i);
			ref_idc = used_by_curr_pic_flag ? 1 : 0;
			if (!used_by_curr_pic_flag) {
				used_by_curr_pic_flag = gf_bs_read_int_log_idx2(bs, 1, "used_by_curr_pic_flag", idx_rps, i);
				ref_idc = used_by_curr_pic_flag << 1;
			}
			if ((ref_idc == 1) || (ref_idc == 2)) {
				s32 deltaPOC = deltaRPS;
				if (i < nb_ref_pics)
					deltaPOC += ref_ps->delta_poc[i];

				rps->delta_poc[k] = deltaPOC;

				if (deltaPOC < 0)  k0++;
				else k1++;

				k++;
			}
		}
		rps->num_negative_pics = k0;
		rps->num_positive_pics = k1;
	}
	else {
		s32 prev = 0, poc;
		sps->rps[idx_rps].num_negative_pics = gf_bs_read_ue_log_idx(bs, "num_negative_pics", idx_rps);
		sps->rps[idx_rps].num_positive_pics = gf_bs_read_ue_log_idx(bs, "num_positive_pics", idx_rps);
		if (sps->rps[idx_rps].num_negative_pics > 16)
			return GF_FALSE;
		if (sps->rps[idx_rps].num_positive_pics > 16)
			return GF_FALSE;
		for (i = 0; i < sps->rps[idx_rps].num_negative_pics; i++) {
			u32 delta_poc_s0_minus1 = gf_bs_read_ue_log_idx2(bs, "delta_poc_s0_minus1", idx_rps, i);
			poc = prev - delta_poc_s0_minus1 - 1;
			prev = poc;
			sps->rps[idx_rps].delta_poc[i] = poc;
			gf_bs_read_int_log_idx2(bs, 1, "delta_poc_s0_minus1", idx_rps, i);
		}
		for (i = 0; i < sps->rps[idx_rps].num_positive_pics; i++) {
			u32 delta_poc_s1_minus1 = gf_bs_read_ue_log_idx2(bs, "delta_poc_s1_minus1" , idx_rps, i);
			poc = prev + delta_poc_s1_minus1 + 1;
			prev = poc;
			sps->rps[idx_rps].delta_poc[i] = poc;
			gf_bs_read_int_log_idx2(bs, 1, "used_by_curr_pic_s1_flag", idx_rps, i);
		}
	}
	return GF_TRUE;
}