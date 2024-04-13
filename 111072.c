u32 gf_id3_read_size(GF_BitStream *bs)
{
	u32 size = 0;
	gf_bs_read_int(bs, 1);
	size |= gf_bs_read_int(bs, 7);
	size<<=7;
	gf_bs_read_int(bs, 1);
	size |= gf_bs_read_int(bs, 7);
	size<<=7;
	gf_bs_read_int(bs, 1);
	size |= gf_bs_read_int(bs, 7);
	size<<=7;
	gf_bs_read_int(bs, 1);
	size |= gf_bs_read_int(bs, 7);
	return size;
}