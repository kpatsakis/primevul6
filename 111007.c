s32 gf_bs_read_se_log_idx2(GF_BitStream *bs, const char *fname, s32 idx1, s32 idx2)
{
	s32 res = gf_bs_read_se(bs);
	if (fname)
		gf_bs_log_idx(bs, -1, fname, res, idx1, idx2, -1);
	return res;
}