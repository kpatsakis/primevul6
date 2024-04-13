static void dec_ref_pic_marking(GF_BitStream *bs, Bool IdrPicFlag) {
	if (IdrPicFlag) {
		gf_bs_read_int_log(bs, 1, "no_output_of_prior_pics_flag");
		gf_bs_read_int_log(bs, 1, "long_term_reference_flag");
	}
	else {
		if (gf_bs_read_int_log(bs, 1, "adaptive_ref_pic_marking_mode_flag")) {
			u32 idx=0, memory_management_control_operation;
			do {
				memory_management_control_operation = gf_bs_read_ue_log_idx(bs, "memory_management_control_operation", idx);
				if (memory_management_control_operation == 1 || memory_management_control_operation == 3)
					gf_bs_read_ue_log_idx(bs, "difference_of_pic_nums_minus1", idx);
				if (memory_management_control_operation == 2)
					gf_bs_read_ue_log_idx(bs, "long_term_pic_num", idx);
				if (memory_management_control_operation == 3 || memory_management_control_operation == 6)
					gf_bs_read_ue_log_idx(bs, "long_term_frame_idx", idx);
				if (memory_management_control_operation == 4)
					gf_bs_read_ue_log_idx(bs, "max_long_term_frame_idx_plus1", idx);
				idx++;
			} while (memory_management_control_operation != 0);
		}
	}
}