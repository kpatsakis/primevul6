static s64 vp9_s(GF_BitStream *bs, int n, const char *fname, u32 idx) {
	s64 value = gf_bs_read_int(bs, n);
	Bool sign = gf_bs_read_int(bs, 1);
	if (sign) value = -value;
	gf_bs_log_idx(bs, n+1, fname, value, idx, -1, -1);
	return value;
}