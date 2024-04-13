u16 gf_bs_read_u16_le(GF_BitStream *bs)
{
	u32 ret, v;
	ret = gf_bs_read_int(bs, 8);
	v = gf_bs_read_int(bs, 8);
	v<<=8;
	ret |= v;
	return ret;
}