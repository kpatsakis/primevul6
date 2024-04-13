static u32 av1_uvlc(GF_BitStream *bs, const char *fname)
{
	u32 res;
	u8 leadingZeros = 0;
	while (1) {
		Bool done = gf_bs_read_int(bs, 1);
		if (done)
			break;
		leadingZeros++;
	}
	if (leadingZeros >= 32) {
		return 0xFFFFFFFF;
	}
	res = gf_bs_read_int(bs, leadingZeros) + (1 << leadingZeros) - 1;
	gf_bs_log(bs, 2*leadingZeros, fname, res);
	return res;
}