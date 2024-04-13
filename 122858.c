u64 gf_bs_set_cookie(GF_BitStream *bs, u64 cookie)
{
	u64 res = 0;
	if (bs) {
		res = bs->cookie;
		bs->cookie = cookie;
	}
	return res;
}