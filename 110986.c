GF_Err gf_hevc_get_sps_info_with_state(HEVCState *hevc, u8 *sps_data, u32 sps_size, u32 *sps_id, u32 *width, u32 *height, s32 *par_n, s32 *par_d)
{
	s32 idx;
	idx = gf_hevc_read_sps(sps_data, sps_size, hevc);
	if (idx < 0) {
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	if (sps_id) *sps_id = idx;

	if (width) *width = hevc->sps[idx].width;
	if (height) *height = hevc->sps[idx].height;
	if (par_n) *par_n = hevc->sps[idx].aspect_ratio_info_present_flag ? hevc->sps[idx].sar_width : (u32)-1;
	if (par_d) *par_d = hevc->sps[idx].aspect_ratio_info_present_flag ? hevc->sps[idx].sar_height : (u32)-1;
	return GF_OK;
}