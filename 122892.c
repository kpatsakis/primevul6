void gf_bs_log_idx(GF_BitStream *bs, u32 nBits, const char *fname, s64 val, s32 idx1, s32 idx2, s32 idx3)
{
	assert(bs);
	if (bs->on_log) bs->on_log(bs->log_udta, fname, nBits, val, idx1, idx2, idx3);
}