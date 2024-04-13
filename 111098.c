static u64 aom_av1_le(GF_BitStream *bs, u32 n, const char *name) {
	u32 i = 0;
	u64 t = 0;
	for (i = 0; i < n; i++) {
		u8 byte = gf_bs_read_int(bs, 8);
		t += (byte << (i * 8));
	}
	gf_bs_log(bs, n*8, name, t);
	return t;
}