s32 gf_hevc_read_vps_bs(GF_BitStream *bs, HEVCState *hevc)
{
	if (!bs || !hevc) return -1;
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);
	if (!hevc_parse_nal_header(bs, NULL, NULL, NULL)) return -1;
	return gf_hevc_read_vps_bs_internal(bs, hevc, GF_FALSE);
}