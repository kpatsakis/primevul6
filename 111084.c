static s32 av1_delta_q(GF_BitStream *bs, const char *name_flag, const char *name)
{
	Bool delta_coded = gf_bs_read_int_log(bs, 1, name_flag);
	s32 delta_q = 0;
	if (delta_coded) {
		u32 signMask = 1 << (7 - 1);
		delta_q = gf_bs_read_int_log(bs, 7, name);
		if (delta_q & signMask)
			delta_q = delta_q - 2 * signMask;
	}
	return delta_q;
}