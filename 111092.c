static void av1_setup_past_independence(AV1State *state)
{
	u32 ref, i;
	for (ref = AV1_LAST_FRAME; ref <= AV1_ALTREF_FRAME; ref++) {
		for (i = 0; i <= 5; i++) {
			state->PrevGmParams.coefs[ref][i] = ((i % 3 == 2) ? 1 << WARPEDMODEL_PREC_BITS : 0);
		}
	}
}