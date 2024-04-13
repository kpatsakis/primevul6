void hevc_profile_tier_level(GF_BitStream *bs, Bool ProfilePresentFlag, u8 MaxNumSubLayersMinus1, HEVC_ProfileTierLevel *ptl, u32 idx)
{
	u32 i;
	if (ProfilePresentFlag) {
		ptl->profile_space = gf_bs_read_int_log_idx(bs, 2, "profile_space", idx);
		ptl->tier_flag = gf_bs_read_int_log_idx(bs, 1, "tier_flag", idx);
		ptl->profile_idc = gf_bs_read_int_log_idx(bs, 5, "profile_idc", idx);

		ptl->profile_compatibility_flag = gf_bs_read_int_log_idx(bs, 32, "profile_compatibility_flag", idx);

		ptl->general_progressive_source_flag = gf_bs_read_int_log_idx(bs, 1, "general_progressive_source_flag", idx);
		ptl->general_interlaced_source_flag = gf_bs_read_int_log_idx(bs, 1, "general_interlaced_source_flag", idx);
		ptl->general_non_packed_constraint_flag = gf_bs_read_int_log_idx(bs, 1, "general_non_packed_constraint_flag", idx);
		ptl->general_frame_only_constraint_flag = gf_bs_read_int_log_idx(bs, 1, "general_frame_only_constraint_flag", idx);
		ptl->general_reserved_44bits = gf_bs_read_long_int(bs, 44);
	}
	ptl->level_idc = gf_bs_read_int_log(bs, 8, "level_idc");
	for (i = 0; i < MaxNumSubLayersMinus1; i++) {
		ptl->sub_ptl[i].profile_present_flag = gf_bs_read_int_log_idx2(bs, 1, "profile_present_flag", idx, i);
		ptl->sub_ptl[i].level_present_flag = gf_bs_read_int_log_idx2(bs, 1, "level_present_flag", idx, i);
	}
	if (MaxNumSubLayersMinus1 > 0) {
		for (i = MaxNumSubLayersMinus1; i < 8; i++) {
			/*reserved_zero_2bits*/gf_bs_read_int(bs, 2);
		}
	}

	for (i = 0; i < MaxNumSubLayersMinus1; i++) {
		if (ptl->sub_ptl[i].profile_present_flag) {
			ptl->sub_ptl[i].profile_space = gf_bs_read_int_log_idx2(bs, 2, "sublayer_profile_space", idx, i);
			ptl->sub_ptl[i].tier_flag = gf_bs_read_int_log_idx2(bs, 1, "sublayer_tier_flag", idx, i);
			ptl->sub_ptl[i].profile_idc = gf_bs_read_int_log_idx2(bs, 5, "sublayer_profile_idc", idx, i);
			ptl->sub_ptl[i].profile_compatibility_flag = gf_bs_read_int_log_idx2(bs, 32, "sublayer_profile_compatibility_flag", idx, i);
			/*ptl->sub_ptl[i].progressive_source_flag =*/ gf_bs_read_int_log_idx2(bs, 1, "sublayer_progressive_source_flag", idx, i);
			/*ptl->sub_ptl[i].interlaced_source_flag =*/ gf_bs_read_int_log_idx2(bs, 1, "sublayer_interlaced_source_flag", idx, i);
			/*ptl->sub_ptl[i].non_packed_constraint_flag =*/ gf_bs_read_int_log_idx2(bs, 1, "sublayer_non_packed_constraint_flag", idx, i);
			/*ptl->sub_ptl[i].frame_only_constraint_flag =*/ gf_bs_read_int_log_idx2(bs, 1, "sublayer_frame_only_constraint_flag", idx, i);
			/*ptl->sub_ptl[i].reserved_44bits =*/ gf_bs_read_long_int(bs, 44);
		}
		if (ptl->sub_ptl[i].level_present_flag)
			ptl->sub_ptl[i].level_idc = gf_bs_read_int_log_idx2(bs, 8, "sublayer_level_idc", idx, i);
	}
}