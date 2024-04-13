s32 gf_hevc_read_vps(u8 *data, u32 size, HEVCState *hevc)
{
	return gf_hevc_read_vps_ex(data, &size, hevc, GF_FALSE);
}