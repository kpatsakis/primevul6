void gf_bs_write_se(GF_BitStream *bs, s32 num)
{
	u32 v;
	if (num <= 0)
		v = (-1 * num) << 1;
	else
		v = (num << 1) - 1;

	gf_bs_write_ue(bs, v);
}