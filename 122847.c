u32 gf_bs_read_u32_le(GF_BitStream *bs)
{
	u32 ret, v;
	ret = gf_bs_read_int(bs, 8);
	v = gf_bs_read_int(bs, 8);
	v<<=8;
	ret |= v;
	v = gf_bs_read_int(bs, 8);
	v<<=16;
	ret |= v;
	v = gf_bs_read_int(bs, 8);
	v<<=24;
	ret |= v;
	return ret;
}