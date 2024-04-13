GF_Err gf_m4a_get_config(u8 *dsi, u32 dsi_size, GF_M4ADecSpecInfo *cfg)
{
	GF_BitStream *bs;
	if (!dsi || !dsi_size || (dsi_size < 2)) return GF_NON_COMPLIANT_BITSTREAM;
	bs = gf_bs_new(dsi, dsi_size, GF_BITSTREAM_READ);
	gf_m4a_parse_config(bs, cfg, GF_TRUE);
	gf_bs_del(bs);
	return GF_OK;
}