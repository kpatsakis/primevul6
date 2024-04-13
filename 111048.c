static void lhvc_parse_rep_format(HEVC_RepFormat *fmt, GF_BitStream *bs, u32 idx)
{
	u8 chroma_bitdepth_present_flag;
	fmt->pic_width_luma_samples = gf_bs_read_int_log_idx(bs, 16, "pic_width_luma_samples", idx);
	fmt->pic_height_luma_samples = gf_bs_read_int_log_idx(bs, 16, "pic_height_luma_samples", idx);
	chroma_bitdepth_present_flag = gf_bs_read_int_log_idx(bs, 1, "chroma_bitdepth_present_flag", idx);
	if (chroma_bitdepth_present_flag) {
		fmt->chroma_format_idc = gf_bs_read_int_log_idx(bs, 2, "chroma_format_idc", idx);

		if (fmt->chroma_format_idc == 3)
			fmt->separate_colour_plane_flag = gf_bs_read_int_log_idx(bs, 1, "separate_colour_plane_flag", idx);
		fmt->bit_depth_luma = 8 + gf_bs_read_int_log_idx(bs, 4, "bit_depth_luma_minus8", idx);
		fmt->bit_depth_chroma = 8 + gf_bs_read_int_log_idx(bs, 4, "bit_depth_chroma_minus8", idx);
	}
	if (gf_bs_read_int_log_idx(bs, 1, "conformance_window_vps_flag", idx)) {
		gf_bs_read_ue_log_idx(bs, "conf_win_vps_left_offset", idx);
		gf_bs_read_ue_log_idx(bs, "conf_win_vps_right_offset", idx);
		gf_bs_read_ue_log_idx(bs, "conf_win_vps_top_offset", idx);
		gf_bs_read_ue_log_idx(bs, "conf_win_vps_bottom_offset", idx);
	}
}