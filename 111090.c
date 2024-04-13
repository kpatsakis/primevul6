GF_Err gf_m4a_write_config(GF_M4ADecSpecInfo *cfg, u8 **dsi, u32 *dsi_size)
{
	GF_BitStream *bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	gf_m4a_write_config_bs(bs, cfg);
	gf_bs_get_content(bs, dsi, dsi_size);
	gf_bs_del(bs);
	return GF_OK;
}