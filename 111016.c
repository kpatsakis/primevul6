Bool ref_pic_lists_modification(GF_BitStream *bs, u32 slice_type, u32 num_ref_idx_l0_active, u32 num_ref_idx_l1_active)
{
	//u32 i;
	Bool ref_pic_list_modification_flag_l0 = gf_bs_read_int_log(bs, 1, "ref_pic_list_modification_flag_l0");
	if (ref_pic_list_modification_flag_l0) {
		/*for (i=0; i<num_ref_idx_l0_active; i++) {
			list_entry_l0[i] = *//*gf_bs_read_int(bs, (u32)ceil(log(getNumPicTotalCurr())/log(2)));
		}*/
		return GF_FALSE;
	}
	if (slice_type == GF_HEVC_SLICE_TYPE_B) {
		Bool ref_pic_list_modification_flag_l1 = gf_bs_read_int_log(bs, 1, "ref_pic_list_modification_flag_l1");
		if (ref_pic_list_modification_flag_l1) {
			/*for (i=0; i<num_ref_idx_l1_active; i++) {
				list_entry_l1[i] = *//*gf_bs_read_int(bs, (u32)ceil(log(getNumPicTotalCurr()) / log(2)));
			}*/
			return GF_FALSE;
		}
	}

	return GF_TRUE;
}