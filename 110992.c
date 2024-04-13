static void av1_color_config(GF_BitStream *bs, AV1State *state)
{
	state->config->high_bitdepth = gf_bs_read_int_log(bs, 1, "high_bitdepth");
	state->bit_depth = 8;
	if (state->config->seq_profile == 2 && state->config->high_bitdepth) {
		state->config->twelve_bit = gf_bs_read_int_log(bs, 1, "twelve_bit");
		state->bit_depth = state->config->twelve_bit ? 12 : 10;
	}
	else if (state->config->seq_profile <= 2) {
		state->bit_depth = state->config->high_bitdepth ? 10 : 8;
	}

	state->config->monochrome = GF_FALSE;
	if (state->config->seq_profile == 1) {
		state->config->monochrome = GF_FALSE;
	}
	else {
		state->config->monochrome = gf_bs_read_int_log(bs, 1, "monochrome");
	}
	/*NumPlanes = mono_chrome ? 1 : 3;*/
	state->color_description_present_flag = gf_bs_read_int_log(bs, 1, "color_description_present_flag");
	if (state->color_description_present_flag) {
		state->color_primaries = gf_bs_read_int_log(bs, 8, "color_primaries");
		state->transfer_characteristics = gf_bs_read_int_log(bs, 8, "transfer_characteristics");
		state->matrix_coefficients = gf_bs_read_int_log(bs, 8, "matrix_coefficients");
	}
	else {
		state->color_primaries = 2/*CP_UNSPECIFIED*/;
		state->transfer_characteristics = 2/*TC_UNSPECIFIED*/;
		state->matrix_coefficients = 2/*MC_UNSPECIFIED*/;
	}
	if (state->config->monochrome) {
		state->color_range = gf_bs_read_int_log(bs, 1, "color_range");
		state->config->chroma_subsampling_x = GF_TRUE;
		state->config->chroma_subsampling_y = GF_TRUE;
		state->config->chroma_sample_position = 0/*CSP_UNKNOWN*/;
		state->separate_uv_delta_q = 0;
		return;
	}
	else if (state->color_primaries == 0/*CP_BT_709*/ &&
		state->transfer_characteristics == 13/*TC_SRGB*/ &&
		state->matrix_coefficients == 0/*MC_IDENTITY*/) {
		state->color_range = GF_TRUE;
		state->config->chroma_subsampling_x = GF_FALSE;
		state->config->chroma_subsampling_y = GF_FALSE;
	}
	else {
		state->config->chroma_subsampling_x = GF_FALSE;
		state->config->chroma_subsampling_y = GF_FALSE;

		state->color_range = gf_bs_read_int_log(bs, 1, "color_range");
		if (state->config->seq_profile == 0) {
			state->config->chroma_subsampling_x = GF_TRUE;
			state->config->chroma_subsampling_y = GF_TRUE;
		}
		else if (state->config->seq_profile == 1) {
			state->config->chroma_subsampling_x = GF_FALSE;
			state->config->chroma_subsampling_y = GF_FALSE;
		}
		else {
			if (state->bit_depth == 12) {
				state->config->chroma_subsampling_x = gf_bs_read_int_log(bs, 1, "chroma_subsampling_x");
				if (state->config->chroma_subsampling_x)
					state->config->chroma_subsampling_y = gf_bs_read_int_log(bs, 1, "chroma_subsampling_y");
				else
					state->config->chroma_subsampling_y = GF_FALSE;
			}
			else {
				state->config->chroma_subsampling_x = GF_TRUE;
				state->config->chroma_subsampling_y = GF_FALSE;
			}
		}
		if (state->config->chroma_subsampling_x && state->config->chroma_subsampling_y) {
			state->config->chroma_sample_position = gf_bs_read_int_log(bs, 2, "chroma_sample_position");
		}
	}
	state->separate_uv_delta_q = gf_bs_read_int_log(bs, 1, "separate_uv_delta_q");
}