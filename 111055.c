s32 gf_avc_read_sps_ext(const char *spse_data, u32 spse_size)
{
	GF_BitStream *bs;
	s32 sps_id;

	bs = gf_bs_new(spse_data, spse_size, GF_BITSTREAM_READ);
	sps_id = gf_avc_read_sps_ext_bs(bs);

	gf_bs_del(bs);
	return sps_id;
}