static void av1_populate_state_from_obu(GF_BitStream *bs, u64 pos, u64 obu_length, ObuType obu_type, AV1State *state)
{
	if (av1_is_obu_header(obu_type)) {
		av1_add_obu_internal(bs, pos, obu_length, obu_type, &state->frame_state.header_obus, NULL);
	}
	if (!state->skip_frames && av1_is_obu_frame(state, obu_type)) {
		if (!state->mem_mode) {
			av1_add_obu_internal(bs, pos, obu_length, obu_type, &state->frame_state.frame_obus, NULL);
		}
		else {
			av1_add_obu_internal(bs, pos, obu_length, obu_type, NULL, state);
		}
	}
}