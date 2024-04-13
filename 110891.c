u64 gf_mpegh_escaped_value(GF_BitStream *bs, u32 nBits1, u32 nBits2, u32 nBits3)
{
	u64 value = gf_bs_read_int(bs, nBits1);
	if (value == (1<<nBits1)-1) {
		u32 vadd = gf_bs_read_int(bs, nBits2);
		value += vadd;
		if (vadd == (1<<nBits2)-1) {
			vadd = gf_bs_read_int(bs, nBits3);
			value += vadd;
		}
	}
	return value;
}