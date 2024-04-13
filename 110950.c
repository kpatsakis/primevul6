static void vp9_loop_filter_params(GF_BitStream *bs)
{
	/*loop_filter_level = */gf_bs_read_int_log(bs, 6, "loop_filter_level");
	/*loop_filter_sharpness = */gf_bs_read_int_log(bs, 3, "loop_filter_sharpness");
	Bool loop_filter_delta_enabled = gf_bs_read_int_log(bs, 1, "loop_filter_delta_enabled");
	if (loop_filter_delta_enabled == 1) {
		Bool loop_filter_delta_update = gf_bs_read_int_log(bs, 1, "loop_filter_delta_update");
		if (loop_filter_delta_update == GF_TRUE) {
			int i;
			for (i = 0; i < 4; i++) {
				Bool update_ref_delta = gf_bs_read_int_log_idx(bs, 1, "update_ref_delta", i);
				if (update_ref_delta == GF_TRUE)
					vp9_s(bs, 6, "loop_filter_ref_deltas", i);
			}
			for (i = 0; i < 2; i++) {
				Bool update_mode_delta = gf_bs_read_int_log_idx(bs, 1, "update_mode_delta", i);
				if (update_mode_delta == GF_TRUE)
					vp9_s(bs, 6, "loop_filter_mode_deltas", i);
			}
		}
	}
}