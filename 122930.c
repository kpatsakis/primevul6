void gf_bs_mark_overflow(GF_BitStream *bs, Bool reset)
{
	bs->overflow_state = reset ? 0 : 2;
}