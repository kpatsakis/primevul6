u64 gf_bs_read_u64_le(GF_BitStream *bs)
{
	u64 ret, v;
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
	v = gf_bs_read_int(bs, 8);
	v<<=32;
	ret |= v;
	v = gf_bs_read_int(bs, 8);
	v<<=40;
	ret |= v;
	v = gf_bs_read_int(bs, 8);
	v<<=48;
	ret |= v;
	v = gf_bs_read_int(bs, 8);
	v<<=56;
	ret |= v;
	return ret;
}