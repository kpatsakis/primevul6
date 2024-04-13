static void decoder_model_info(AV1State *state, GF_BitStream *bs) {
	state->buffer_delay_length = 1 + gf_bs_read_int_log(bs, 5, "buffer_delay_length_minus1");
	gf_bs_read_int_log(bs, 32, "num_units_in_decoding_tick");
	state->buffer_removal_time_length = gf_bs_read_int_log(bs, 5, "buffer_removal_time_length");
	state->frame_presentation_time_length = 1 + gf_bs_read_int_log(bs, 5, "frame_presentation_time_length_minus1");
}