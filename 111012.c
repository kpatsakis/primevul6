static void ref_pic_list_modification(GF_BitStream *bs, u32 slice_type) {
	if (slice_type % 5 != 2 && slice_type % 5 != 4) {
		if (gf_bs_read_int_log(bs, 1, "ref_pic_list_modification_flag_l0")) {
			u32 idx=0, modification_of_pic_nums_idc;
			do {
				modification_of_pic_nums_idc = gf_bs_read_ue_log_idx(bs, "modification_of_pic_nums_idc", idx);
				if (modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1) {
					gf_bs_read_ue_log_idx(bs, "abs_diff_pic_num_minus1", idx);
				}
				else if (modification_of_pic_nums_idc == 2) {
					gf_bs_read_ue_log_idx(bs, "long_term_pic_num", idx);
				}
				idx++;
			} while ((modification_of_pic_nums_idc != 3) && gf_bs_available(bs));
		}
	}
	if (slice_type % 5 == 1) {
		if (gf_bs_read_int_log(bs, 1, "ref_pic_list_modification_flag_l1")) {
			u32 idx=0, modification_of_pic_nums_idc;
			do {
				modification_of_pic_nums_idc = gf_bs_read_ue_log_idx(bs, "modification_of_pic_nums_idc", idx);
				if (modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1) {
					gf_bs_read_ue_log_idx(bs, "abs_diff_pic_num_minus1", idx);
				}
				else if (modification_of_pic_nums_idc == 2) {
					gf_bs_read_ue_log_idx(bs, "long_term_pic_num", idx);
				}
				idx++;
			} while ((modification_of_pic_nums_idc != 3) && gf_bs_available(bs));
		}
	}
}