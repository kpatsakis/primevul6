static s32 avc_parse_pic_timing_sei(GF_BitStream *bs, AVCState *avc)
{
	int sps_id = avc->sps_active_idx;
	const char NumClockTS[] = { 1, 1, 1, 2, 2, 3, 3, 2, 3 };
	AVCSeiPicTiming *pt = &avc->sei.pic_timing;

	if (sps_id < 0) {
		/*sps_active_idx equals -1 when no sps has been detected. In this case SEI should not be decoded.*/
		assert(0);
		return 1;
	}
	if (avc->sps[sps_id].vui.nal_hrd_parameters_present_flag || avc->sps[sps_id].vui.vcl_hrd_parameters_present_flag) { /*CpbDpbDelaysPresentFlag, see 14496-10(2003) E.11*/
		gf_bs_read_int_log(bs, 1 + avc->sps[sps_id].vui.hrd.cpb_removal_delay_length_minus1, "cpb_removal_delay_minus1");
		gf_bs_read_int_log(bs, 1 + avc->sps[sps_id].vui.hrd.dpb_output_delay_length_minus1, "dpb_output_delay_minus1");
	}

	/*ISO 14496-10 (2003), D.8.2: we need to get pic_struct in order to know if we display top field first or bottom field first*/
	if (avc->sps[sps_id].vui.pic_struct_present_flag) {
		int i;
		pt->pic_struct = gf_bs_read_int_log(bs, 4, "pic_struct");
		if (pt->pic_struct > 8) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[avc-h264] invalid pic_struct value %d\n", pt->pic_struct));
			return 1;
		}

		for (i = 0; i < NumClockTS[pt->pic_struct]; i++) {
			if (gf_bs_read_int_log_idx(bs, 1, "clock_timestamp_flag", i)) {
				Bool full_timestamp_flag;
				gf_bs_read_int_log_idx(bs, 2, "ct_type", i);
				gf_bs_read_int_log_idx(bs, 1, "nuit_field_based_flag", i);
				gf_bs_read_int_log_idx(bs, 5, "counting_type", i);
				full_timestamp_flag = gf_bs_read_int_log_idx(bs, 1, "full_timestamp_flag", i);
				gf_bs_read_int_log_idx(bs, 1, "discontinuity_flag", i);
				gf_bs_read_int_log_idx(bs, 1, "cnt_dropped_flag", i);
				gf_bs_read_int_log_idx(bs, 8, "n_frames", i);
				if (full_timestamp_flag) {
					gf_bs_read_int_log_idx(bs, 6, "seconds_value", i);
					gf_bs_read_int_log_idx(bs, 6, "minutes_value", i);
					gf_bs_read_int_log_idx(bs, 5, "hours_value", i);
				}
				else {
					if (gf_bs_read_int_log_idx(bs, 1, "seconds_flag", i)) {
						gf_bs_read_int_log_idx(bs, 6, "seconds_value", i);
						if (gf_bs_read_int_log_idx(bs, 1, "minutes_flag", i)) {
							gf_bs_read_int_log_idx(bs, 6, "minutes_value", i);
							if (gf_bs_read_int_log_idx(bs, 1, "hours_flag", i)) {
								gf_bs_read_int_log_idx(bs, 5, "hours_value", i);
							}
						}
					}
					if (avc->sps[sps_id].vui.hrd.time_offset_length > 0)
						gf_bs_read_int_log_idx(bs, avc->sps[sps_id].vui.hrd.time_offset_length, "time_offset", i);
				}
			}
		}
	}

	return 0;
}