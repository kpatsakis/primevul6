static void av1_frame_size(GF_BitStream *bs, AV1State *state, Bool frame_size_override_flag)
{
	if (frame_size_override_flag) {
		u32 frame_width_minus_1, frame_height_minus_1;
		u8 n = state->frame_width_bits_minus_1 + 1;
		frame_width_minus_1 = gf_bs_read_int_log(bs, n, "frame_width_minus_1");
		n = state->frame_height_bits_minus_1 + 1;
		frame_height_minus_1 = gf_bs_read_int_log(bs, n, "frame_height_minus_1");
		state->width = frame_width_minus_1 + 1;
		state->height = frame_height_minus_1 + 1;
	} else {
		state->width = state->sequence_width;
		state->height = state->sequence_height;
	}
	superres_params(bs, state);
	//compute_image_size(); //no bits
}