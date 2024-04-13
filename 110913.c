static void on_aom_av1_eos(void *_state)
{
	AV1State *state = (AV1State *)_state;
	state->bs_overread = GF_TRUE;
}