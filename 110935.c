static void avc_compute_poc(AVCSliceInfo *si)
{
	enum {
		AVC_PIC_FRAME,
		AVC_PIC_FIELD_TOP,
		AVC_PIC_FIELD_BOTTOM,
	} pic_type;
	s32 field_poc[2] = { 0,0 };
	s32 max_frame_num;

	if (!si->sps) return;

	max_frame_num = 1 << (si->sps->log2_max_frame_num);

	/* picture type */
	if (si->sps->frame_mbs_only_flag || !si->field_pic_flag) pic_type = AVC_PIC_FRAME;
	else if (si->bottom_field_flag) pic_type = AVC_PIC_FIELD_BOTTOM;
	else pic_type = AVC_PIC_FIELD_TOP;

	/* frame_num_offset */
	if (si->nal_unit_type == GF_AVC_NALU_IDR_SLICE) {
		si->poc_lsb_prev = 0;
		si->poc_msb_prev = 0;
		si->frame_num_offset = 0;
	}
	else {
		if (si->frame_num < si->frame_num_prev)
			si->frame_num_offset = si->frame_num_offset_prev + max_frame_num;
		else
			si->frame_num_offset = si->frame_num_offset_prev;
	}

	/*ISO 14496-10 N.11084 8.2.1.1*/
	if (si->sps->poc_type == 0)
	{
		const u32 max_poc_lsb = 1 << (si->sps->log2_max_poc_lsb);

		/*ISO 14496-10 N.11084 eq (8-3)*/
		if ((si->poc_lsb < si->poc_lsb_prev) &&
			(si->poc_lsb_prev - si->poc_lsb >= max_poc_lsb / 2))
			si->poc_msb = si->poc_msb_prev + max_poc_lsb;
		else if ((si->poc_lsb > si->poc_lsb_prev) &&
			(si->poc_lsb - si->poc_lsb_prev > max_poc_lsb / 2))
			si->poc_msb = si->poc_msb_prev - max_poc_lsb;
		else
			si->poc_msb = si->poc_msb_prev;

		/*ISO 14496-10 N.11084 eq (8-4)*/
		if (pic_type != AVC_PIC_FIELD_BOTTOM)
			field_poc[0] = si->poc_msb + si->poc_lsb;

		/*ISO 14496-10 N.11084 eq (8-5)*/
		if (pic_type != AVC_PIC_FIELD_TOP) {
			if (!si->field_pic_flag)
				field_poc[1] = field_poc[0] + si->delta_poc_bottom;
			else
				field_poc[1] = si->poc_msb + si->poc_lsb;
		}
	}
	/*ISO 14496-10 N.11084 8.2.1.2*/
	else if (si->sps->poc_type == 1)
	{
		u32 i;
		s32 abs_frame_num, expected_delta_per_poc_cycle, expected_poc;

		if (si->sps->poc_cycle_length)
			abs_frame_num = si->frame_num_offset + si->frame_num;
		else
			abs_frame_num = 0;

		if (!si->nal_ref_idc && (abs_frame_num > 0)) abs_frame_num--;

		expected_delta_per_poc_cycle = 0;
		for (i = 0; i < si->sps->poc_cycle_length; i++)
			expected_delta_per_poc_cycle += si->sps->offset_for_ref_frame[i];

		if (abs_frame_num > 0) {
			const u32 poc_cycle_cnt = (abs_frame_num - 1) / si->sps->poc_cycle_length;
			const u32 frame_num_in_poc_cycle = (abs_frame_num - 1) % si->sps->poc_cycle_length;

			expected_poc = poc_cycle_cnt * expected_delta_per_poc_cycle;
			for (i = 0; i <= frame_num_in_poc_cycle; i++)
				expected_poc += si->sps->offset_for_ref_frame[i];
		}
		else {
			expected_poc = 0;
		}

		if (!si->nal_ref_idc) expected_poc += si->sps->offset_for_non_ref_pic;

		field_poc[0] = expected_poc + si->delta_poc[0];
		field_poc[1] = field_poc[0] + si->sps->offset_for_top_to_bottom_field;
		if (pic_type == AVC_PIC_FRAME) field_poc[1] += si->delta_poc[1];
	}
	/*ISO 14496-10 N.11084 8.2.1.3*/
	else if (si->sps->poc_type == 2)
	{
		int poc;
		if (si->nal_unit_type == GF_AVC_NALU_IDR_SLICE) {
			poc = 0;
		}
		else {
			const int abs_frame_num = si->frame_num_offset + si->frame_num;
			poc = 2 * abs_frame_num;
			if (!si->nal_ref_idc) poc -= 1;
		}
		field_poc[0] = poc;
		field_poc[1] = poc;
	}

	/*ISO 14496-10 N.11084 eq (8-1)*/
	if (pic_type == AVC_PIC_FRAME)
		si->poc = MIN(field_poc[0], field_poc[1]);
	else if (pic_type == AVC_PIC_FIELD_TOP)
		si->poc = field_poc[0];
	else
		si->poc = field_poc[1];
}