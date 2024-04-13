void gf_bs_write_u32_le(GF_BitStream *bs, u32 val)
{
	gf_bs_write_int(bs, val & 0xFF, 8);
	gf_bs_write_int(bs, val>>8, 8);
	gf_bs_write_int(bs, val>>16, 8);
	gf_bs_write_int(bs, val>>24, 8);
}