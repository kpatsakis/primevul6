void gf_av1_init_state(AV1State *state)
{
	if (!state) return;
	memset(state, 0, sizeof(AV1State));
	state->color_primaries = 2;
	state->transfer_characteristics = 2;
	state->matrix_coefficients = 2;
}