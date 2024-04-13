static s32 av1_decode_unsigned_subexp_with_ref(GF_BitStream *bs, s32 mx, s32 r)
{
	u32 v = av1_decode_subexp(bs, mx);
	if ((r < 0) && (-(-r << 1) <= mx)) {
		return inverse_recenter(r, v);
	}
	else if ((r << 1) <= mx) {
		return inverse_recenter(r, v);
	}
	else {
		return mx - 1 - inverse_recenter(mx - 1 - r, v);
	}
}