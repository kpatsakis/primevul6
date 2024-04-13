s32 gf_hevc_read_pps(u8 *data, u32 size, HEVCState *hevc)
{
	GF_BitStream *bs;
	s32 pps_id = -1;

	bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
	if (!bs) goto exit;
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);

	if (!hevc_parse_nal_header(bs, NULL, NULL, NULL)) goto exit;

	pps_id = gf_hevc_read_pps_bs_internal(bs, hevc);

exit:
	if (bs) gf_bs_del(bs);
	return pps_id;
}