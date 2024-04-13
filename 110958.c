u32 gf_bs_read_int_log_idx3(GF_BitStream *bs, u32 nBits, const char *fname, s32 idx1, s32 idx2, s32 idx3)
{
	u32 val = gf_bs_read_int(bs, nBits);
	gf_bs_log_idx(bs, nBits, fname, val, idx1, idx2, idx3);
	return val;
}