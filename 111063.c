s32 gf_bs_read_se(GF_BitStream *bs)
{
	u32 v = gf_bs_read_ue(bs);
	if ((v & 0x1) == 0) return (s32)(0 - (v >> 1));
	return (v + 1) >> 1;
}