void gf_bs_write_u64_le(GF_BitStream *bs, u64 val)
{
	gf_bs_write_int(bs, val & 0xFF, 8);
	gf_bs_write_int(bs, (val>>8) & 0xFF, 8);
	gf_bs_write_int(bs, (val>>16) & 0xFF, 8);
	gf_bs_write_int(bs, (val>>24) & 0xFF, 8);
	gf_bs_write_int(bs, (val>>32) & 0xFF, 8);
	gf_bs_write_int(bs, (val>>40) & 0xFF, 8);
	gf_bs_write_int(bs, (val>>48) & 0xFF, 8);
	gf_bs_write_int(bs, (val>>56) & 0xFF, 8);
}