static void av1_read_global_param(AV1State *state, GF_BitStream *bs, u8 type, u8 ref, u8 idx)
{
	u8 absBits = GM_ABS_ALPHA_BITS;
	u8 precBits = GM_ALPHA_PREC_BITS;
	if (idx < 2) {
		if (type == AV1_GMC_TRANSLATION) {
			absBits = GM_ABS_TRANS_ONLY_BITS - (!state->frame_state.allow_high_precision_mv ? 1 : 0);
			precBits = GM_TRANS_ONLY_PREC_BITS - (!state->frame_state.allow_high_precision_mv ? 1 : 0);
		}
		else {
			absBits = GM_ABS_TRANS_BITS;
			precBits = GM_TRANS_PREC_BITS;
		}
	}
	s32 precDiff = WARPEDMODEL_PREC_BITS - precBits;
	s32 round = (idx % 3) == 2 ? (1 << WARPEDMODEL_PREC_BITS) : 0;
	s32 sub = (idx % 3) == 2 ? (1 << precBits) : 0;
	s32 mx = (1 << absBits);
	s32 r = (state->PrevGmParams.coefs[ref][idx] >> precDiff) - sub;
	s32 val = av1_decode_signed_subexp_with_ref(bs, -mx, mx + 1, r);

	if (val < 0) {
		val = -val;
		state->GmParams.coefs[ref][idx] = (-(val << precDiff) + round);
	}
	else {
		state->GmParams.coefs[ref][idx] = (val << precDiff) + round;
	}
}