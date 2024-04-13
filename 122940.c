u64 gf_bs_get_cookie(GF_BitStream *bs)
{
	if (!bs) return 0;
	return bs->cookie;
}