s32 gf_hevc_read_sps(u8 *data, u32 size, HEVCState *hevc)
{
	return gf_hevc_read_sps_ex(data, size, hevc, NULL);
}