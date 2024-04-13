s32 gf_hevc_read_sps_bs(GF_BitStream *bs, HEVCState *hevc)
{
	u8 layer_id;
	if (!bs || !hevc) return -1;
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);
	if (!hevc_parse_nal_header(bs, NULL, NULL, &layer_id)) return -1;
	return gf_hevc_read_sps_bs_internal(bs, hevc, layer_id, NULL);
}