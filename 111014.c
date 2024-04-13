static void sub_layer_hrd_parameters(GF_BitStream *bs, int subLayerId, u32 cpb_cnt, Bool sub_pic_hrd_params_present_flag, u32 idx1, u32 idx2)
{
	u32 i;
	if (!gf_bs_available(bs)) return;

	for (i = 0; i <= cpb_cnt; i++) {
		gf_bs_read_ue_log_idx3(bs, "bit_rate_value_minus1", idx1, idx2, i);
		gf_bs_read_ue_log_idx3(bs, "cpb_size_value_minus1", idx1, idx2, i);
		if (sub_pic_hrd_params_present_flag) {
			gf_bs_read_ue_log_idx3(bs, "cpb_size_du_value_minus1", idx1, idx2, i);
			gf_bs_read_ue_log_idx3(bs, "bit_rate_du_value_minus1", idx1, idx2, i);
		}
		gf_bs_read_int_log_idx3(bs, 1, "cbr_flag", idx1, idx2, i);
	}
}