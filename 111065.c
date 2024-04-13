static void hevc_parse_hrd_parameters(GF_BitStream *bs, Bool commonInfPresentFlag, int maxNumSubLayersMinus1, u32 idx)
{
	int i;
	Bool nal_hrd_parameters_present_flag = GF_FALSE;
	Bool vcl_hrd_parameters_present_flag = GF_FALSE;
	Bool sub_pic_hrd_params_present_flag = GF_FALSE;

	if (commonInfPresentFlag) {
		nal_hrd_parameters_present_flag = gf_bs_read_int_log_idx(bs, 1, "nal_hrd_parameters_present_flag", idx);
		vcl_hrd_parameters_present_flag = gf_bs_read_int_log_idx(bs, 1, "vcl_hrd_parameters_present_flag", idx);
		if (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag) {
			sub_pic_hrd_params_present_flag = gf_bs_read_int_log_idx(bs, 1, "sub_pic_hrd_params_present_flag", idx);
			if (sub_pic_hrd_params_present_flag) {
				gf_bs_read_int_log_idx(bs, 8, "tick_divisor_minus2", idx);
				gf_bs_read_int_log_idx(bs, 5, "du_cpb_removal_delay_increment_length_minus1", idx);
				gf_bs_read_int_log_idx(bs, 1, "sub_pic_cpb_params_in_pic_timing_sei_flag", idx);
				gf_bs_read_int_log_idx(bs, 5, "dpb_output_delay_du_length_minus1", idx);
			}
			gf_bs_read_int_log_idx(bs, 4, "bit_rate_scale", idx);
			gf_bs_read_int_log_idx(bs, 4, "cpb_size_scale", idx);
			if (sub_pic_hrd_params_present_flag) {
				gf_bs_read_int_log_idx(bs, 4, "cpb_size_du_scale", idx);
			}
			gf_bs_read_int_log_idx(bs, 5, "initial_cpb_removal_delay_length_minus1", idx);
			gf_bs_read_int_log_idx(bs, 5, "au_cpb_removal_delay_length_minus1", idx);
			gf_bs_read_int_log_idx(bs, 5, "dpb_output_delay_length_minus1", idx);
		}
	}
	for (i = 0; i <= maxNumSubLayersMinus1; i++) {
		Bool fixed_pic_rate_general_flag_i = gf_bs_read_int_log_idx(bs, 1, "fixed_pic_rate_general_flag", idx);
		Bool fixed_pic_rate_within_cvs_flag_i = GF_TRUE;
		Bool low_delay_hrd_flag_i = GF_FALSE;
		u32 cpb_cnt_minus1_i = 0;
		if (!fixed_pic_rate_general_flag_i) {
			fixed_pic_rate_within_cvs_flag_i = gf_bs_read_int_log_idx(bs, 1, "fixed_pic_rate_within_cvs_flag", idx);
		}
		if (fixed_pic_rate_within_cvs_flag_i)
			gf_bs_read_ue_log_idx(bs, "elemental_duration_in_tc_minus1", idx);
		else
			low_delay_hrd_flag_i = gf_bs_read_int_log_idx(bs, 1, "low_delay_hrd_flag", idx);
		if (!low_delay_hrd_flag_i) {
			cpb_cnt_minus1_i = gf_bs_read_ue_log_idx(bs, "cpb_cnt_minus1", idx);
		}
		if (nal_hrd_parameters_present_flag) {
			sub_layer_hrd_parameters(bs, i, cpb_cnt_minus1_i, sub_pic_hrd_params_present_flag, idx, i);
		}
		if (vcl_hrd_parameters_present_flag) {
			sub_layer_hrd_parameters(bs, i, cpb_cnt_minus1_i, sub_pic_hrd_params_present_flag, idx, i);
		}
	}
}