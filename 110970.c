static void av1_parse_sequence_header_obu(GF_BitStream *bs, AV1State *state)
{
	u8 buffer_delay_length_minus_1 = 0;
	state->frame_state.seen_seq_header = GF_TRUE;
	state->config->seq_profile = gf_bs_read_int_log(bs, 3, "seq_profile");
	state->still_picture = gf_bs_read_int_log(bs, 1, "still_picture");
	state->reduced_still_picture_header = gf_bs_read_int_log(bs, 1, "reduced_still_picture_header");
	if (state->reduced_still_picture_header) {
		//timing_info_present_flag = GF_FALSE;
		//initial_display_delay_present_flag = GF_FALSE;
		state->operating_points_count = 1;
		state->config->seq_level_idx_0 = gf_bs_read_int_log(bs, 5, "seq_level_idx_0");
	}
	else {
		u8 i = 0;
		Bool initial_display_delay_present_flag;
		Bool timing_info_present_flag = gf_bs_read_int_log(bs, 1, "timing_info_present_flag");
		if (timing_info_present_flag) {
			timing_info(bs, state);
			state->decoder_model_info_present_flag = gf_bs_read_int_log(bs, 1, "decoder_model_info_present_flag");
			if (state->decoder_model_info_present_flag) {
				decoder_model_info(state, bs);
			}
		}
		else {
			state->decoder_model_info_present_flag = GF_FALSE;
		}
		initial_display_delay_present_flag = gf_bs_read_int_log(bs, 1, "initial_display_delay_present_flag");
		state->operating_points_count = 1 + gf_bs_read_int_log(bs, 5, "operating_points_count_minus1");
		for (i = 0; i < state->operating_points_count; i++) {
			u8 seq_level_idx_i, seq_tier = 0;

			state->operating_point_idc[i] = gf_bs_read_int_log_idx(bs, 12, "operating_point_idc", i);

			seq_level_idx_i = gf_bs_read_int_log_idx(bs, 5, "seq_level_idx", i);
			if (i == 0) state->config->seq_level_idx_0 = seq_level_idx_i;

			if (seq_level_idx_i > 7) {
				seq_tier = gf_bs_read_int_log_idx(bs, 1, "seq_tier", i);
			}
			if (i == 0) state->config->seq_tier_0 = seq_tier;

			if (state->decoder_model_info_present_flag) {
				state->decoder_model_present_for_this_op[i] = gf_bs_read_int_log_idx(bs, 1, "decoder_model_present_for_this_op", i);
				if (state->decoder_model_present_for_this_op[i]) {
					operating_parameters_info(bs, i, buffer_delay_length_minus_1);
				}
			}
			else {
				state->decoder_model_present_for_this_op[i] = 0;
			}
			if (initial_display_delay_present_flag) {
				if (gf_bs_read_int_log_idx(bs, 1, "initial_display_delay_present_for_this_op", i) ) {
					gf_bs_read_int_log_idx(bs, 4, "initial_display_delay_minus1", i);
				}
			}
		}
	}

	//operatingPoint = av1_choose_operating_point(bs);
	state->OperatingPointIdc = 0;//TODO: operating_point_idc[operatingPoint];

	state->frame_width_bits_minus_1 = gf_bs_read_int_log(bs, 4, "frame_width_bits_minus1");
	state->frame_height_bits_minus_1 = gf_bs_read_int_log(bs, 4, "frame_height_bits_minus1");
	state->width = gf_bs_read_int_log(bs, state->frame_width_bits_minus_1 + 1, "width_minus1") + 1;
	state->height = gf_bs_read_int_log(bs, state->frame_height_bits_minus_1 + 1, "height_minus1") + 1;
	state->sequence_width = state->width;
	state->sequence_height = state->height;
	state->frame_id_numbers_present_flag = GF_FALSE;
	if (!state->reduced_still_picture_header) {
		state->frame_id_numbers_present_flag = gf_bs_read_int_log(bs, 1, "frame_id_numbers_present_flag");
	}
	if (state->frame_id_numbers_present_flag) {
		state->delta_frame_id_length_minus_2 = gf_bs_read_int_log(bs, 4, "delta_frame_id_length_minus2");
		state->additional_frame_id_length_minus_1 = gf_bs_read_int_log(bs, 3, "additional_frame_id_length_minus1");
	}
	state->use_128x128_superblock = gf_bs_read_int_log(bs, 1, "use_128x128_superblock");
	gf_bs_read_int_log(bs, 1, "enable_filter_intra");
	gf_bs_read_int_log(bs, 1, "enable_intra_edge_filter");
	if (state->reduced_still_picture_header) {
		/*enable_interintra_compound = 0;
		enable_masked_compound = 0;
		enable_dual_filter = 0;
		enable_jnt_comp = 0;
		enable_ref_frame_mvs = 0;*/
		state->enable_warped_motion = 0;
		state->enable_order_hint = GF_FALSE;
		state->OrderHintBits = 0;
		state->seq_force_integer_mv = 2/*SELECT_INTEGER_MV*/;
		state->seq_force_screen_content_tools = 2/*SELECT_SCREEN_CONTENT_TOOLS*/;
	}
	else {
		Bool seq_choose_screen_content_tools;
		gf_bs_read_int_log(bs, 1, "enable_interintra_compound");
		gf_bs_read_int_log(bs, 1, "enable_masked_compound");
		state->enable_warped_motion = gf_bs_read_int_log(bs, 1, "enable_warped_motion");
		gf_bs_read_int_log(bs, 1, "enable_dual_filter");
		state->enable_order_hint = gf_bs_read_int_log(bs, 1, "enable_order_hint");
		if (state->enable_order_hint) {
			gf_bs_read_int_log(bs, 1, "enable_jnt_comp");
			state->enable_ref_frame_mvs = gf_bs_read_int_log(bs, 1, "enable_ref_frame_mvs");
		}
		else {
			/*enable_jnt_comp =  0*/;
			/*enable_ref_frame_mvs = 0*/;
		}
		seq_choose_screen_content_tools = gf_bs_read_int_log(bs, 1, "seq_choose_screen_content_tools");
		state->seq_force_screen_content_tools = 0;
		if (seq_choose_screen_content_tools) {
			state->seq_force_screen_content_tools = 2/*SELECT_SCREEN_CONTENT_TOOLS*/;
		}
		else {
			state->seq_force_screen_content_tools = gf_bs_read_int_log(bs, 1, "seq_force_screen_content_tools");
		}

		state->seq_force_integer_mv = 0;
		if (state->seq_force_screen_content_tools > 0) {
			const Bool seq_choose_integer_mv = gf_bs_read_int_log(bs, 1, "seq_choose_integer_mv");
			if (seq_choose_integer_mv) {
				state->seq_force_integer_mv = 2/*SELECT_INTEGER_MV*/;
			}
			else {
				state->seq_force_integer_mv = gf_bs_read_int_log(bs, 1, "seq_force_integer_mv");
			}
		}
		else {
			state->seq_force_integer_mv = 2/*SELECT_INTEGER_MV*/;
		}
		if (state->enable_order_hint) {
			u8 order_hint_bits_minus_1 = gf_bs_read_int_log(bs, 3, "order_hint_bits_minus1");
			state->OrderHintBits = order_hint_bits_minus_1 + 1;
		}
		else {
			state->OrderHintBits = 0;
		}
	}

	state->enable_superres = gf_bs_read_int_log(bs, 1, "enable_superres");
	state->enable_cdef = gf_bs_read_int_log(bs, 1, "enable_cdef");
	state->enable_restoration = gf_bs_read_int_log(bs, 1, "enable_restoration");
	av1_color_config(bs, state);
	state->film_grain_params_present = gf_bs_read_int_log(bs, 1, "film_grain_params_present");
}