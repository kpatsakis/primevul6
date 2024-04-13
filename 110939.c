static s16 av1_decode_signed_subexp_with_ref(GF_BitStream *bs, s32 low, s32 high, s32 r)
{
	s16 x = av1_decode_unsigned_subexp_with_ref(bs, high - low, r - low);
	return x + low;
}