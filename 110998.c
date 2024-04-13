static void timing_info(GF_BitStream *bs, AV1State *state) {
	u32 time_scale = 0;
	u32 num_units_in_display_tick = gf_bs_read_int_log(bs, 32, "num_units_in_display_tick");
	if (num_units_in_display_tick == 0) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AV1] num_units_in_display_tick must be greater than 0.\n"));
	}
	time_scale = gf_bs_read_int_log(bs, 32, "time_scale");
	if (time_scale == 0) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AV1] time_scale must be greater than 0.\n"));
	}
	state->equal_picture_interval = gf_bs_read_int_log(bs, 1, "equal_picture_interval");
	if (state->equal_picture_interval) {
		u32 num_ticks_per_picture_minus_1 = av1_uvlc(bs, "num_ticks_per_picture_minus_1");
		state->tb_num = time_scale;
		state->tb_den = (num_ticks_per_picture_minus_1 + 1)*num_units_in_display_tick;
	}
	else {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AV1] VFR not supported.\n"));
		//TODO: upload num_units_in_display_tick (eq. to the POC in H264), compute delta between frames, set it as dts_inc in gf_import_aom_av1()
	}
}