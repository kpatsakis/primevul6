GF_Err gf_hevc_get_sps_info(u8 *sps_data, u32 sps_size, u32 *sps_id, u32 *width, u32 *height, s32 *par_n, s32 *par_d)
{
	HEVCState hevc;
	memset(&hevc, 0, sizeof(HEVCState));
	hevc.sps_active_idx = -1;
	return gf_hevc_get_sps_info_with_state(&hevc, sps_data, sps_size, sps_id, width, height, par_n, par_d);
}