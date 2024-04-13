s32 gf_hevc_read_sps_ex(char *data, u32 size, HEVCState *hevc, u32 *vui_flag_pos)
{
	GF_BitStream *bs;
	s32 sps_id = -1;
	u8 layer_id;

	if (vui_flag_pos) *vui_flag_pos = 0;

	bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
	if (!bs) goto exit;
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);

	if (!hevc_parse_nal_header(bs, NULL, NULL, &layer_id)) goto exit;
	sps_id = gf_hevc_read_sps_bs_internal(bs, hevc, layer_id, vui_flag_pos);

exit:
	if (bs) gf_bs_del(bs);
	return sps_id;
}