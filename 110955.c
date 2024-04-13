static void superres_params(GF_BitStream *bs, AV1State *state)
{
	u32 SuperresDenom;
	Bool use_superres;

	if (state->enable_superres) {
		use_superres = gf_bs_read_int_log(bs, 1, "use_superres");
	}
	else {
		use_superres = GF_FALSE;
	}
	if (use_superres) {
		u8 coded_denom = gf_bs_read_int_log(bs, AV1_SUPERRES_DENOM_BITS, "coded_denom");
		SuperresDenom = coded_denom + AV1_SUPERRES_DENOM_MIN;
	}
	else {
		SuperresDenom = AV1_SUPERRES_NUM;
	}
	state->UpscaledWidth = state->width;
	state->width = (state->UpscaledWidth * AV1_SUPERRES_NUM + (SuperresDenom / 2)) / SuperresDenom;
}