GF_Err gf_avc_get_sps_info(u8 *sps_data, u32 sps_size, u32 *sps_id, u32 *width, u32 *height, s32 *par_n, s32 *par_d)
{
	AVCState avc;
	s32 idx;
	memset(&avc, 0, sizeof(AVCState));
	avc.sps_active_idx = -1;

	idx = gf_avc_read_sps(sps_data, sps_size, &avc, 0, NULL);
	if (idx < 0) {
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	if (sps_id) *sps_id = idx;

	if (width) *width = avc.sps[idx].width;
	if (height) *height = avc.sps[idx].height;
	if (par_n) *par_n = avc.sps[idx].vui.par_num ? avc.sps[idx].vui.par_num : (u32)-1;
	if (par_d) *par_d = avc.sps[idx].vui.par_den ? avc.sps[idx].vui.par_den : (u32)-1;

	return GF_OK;
}