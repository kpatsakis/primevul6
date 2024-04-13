s32 gf_avc_read_pps(const u8 *pps_data, u32 pps_size, AVCState *avc)
{
	GF_BitStream *bs;
	s32 pps_id;

	/*PPS still contains emulation bytes*/
	bs = gf_bs_new(pps_data, pps_size, GF_BITSTREAM_READ);
	if (!bs) {
		return -1;
	}
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);
	pps_id = gf_avc_read_pps_bs(bs, avc);
	gf_bs_del(bs);
	return pps_id;
}