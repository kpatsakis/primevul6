static void av1_parse_frame_header(GF_BitStream *bs, AV1State *state)
{
	AV1StateFrame *frame_state = &state->frame_state;
	if (frame_state->seen_frame_header == GF_FALSE) {
		u64 pos = gf_bs_get_position(bs);
		state->frame_state.show_existing_frame = GF_FALSE;
		frame_state->seen_frame_header = GF_TRUE;
		av1_parse_uncompressed_header(bs, state);
		state->frame_state.is_first_frame = GF_FALSE;
		state->frame_state.uncompressed_header_bytes = (u32) (gf_bs_get_position(bs) - pos);

		if (state->frame_state.show_existing_frame) {
			av1_decode_frame_wrapup(state);
			frame_state->seen_frame_header = GF_FALSE;
		}
		else {
			//TileNum = 0;
			frame_state->seen_frame_header = GF_TRUE;
		}
	}
}