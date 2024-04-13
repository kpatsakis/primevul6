GF_Err gf_avc_get_pps_info(u8 *pps_data, u32 pps_size, u32 *pps_id, u32 *sps_id)
{
	GF_BitStream *bs;
	GF_Err e = GF_OK;

	bs = gf_bs_new(pps_data, pps_size, GF_BITSTREAM_READ);
	if (!bs) {
		e = GF_NON_COMPLIANT_BITSTREAM;
		goto exit;
	}
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);
	/*nal hdr*/ gf_bs_read_int(bs, 8);

	*pps_id = gf_bs_read_ue(bs);
	*sps_id = gf_bs_read_ue(bs);

exit:
	gf_bs_del(bs);
	return e;
}