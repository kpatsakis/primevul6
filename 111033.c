GF_Err gf_m4v_parse_frame(GF_M4VParser *m4v, GF_M4VDecSpecInfo *dsi, u8 *frame_type, u32 *time_inc, u64 *size, u64 *start, Bool *is_coded)
{
	if (m4v->mpeg12) {
		return gf_m4v_parse_frame_mpeg12(m4v, dsi, frame_type, time_inc, size, start, is_coded);
	}
	else {
		return gf_m4v_parse_frame_mpeg4(m4v, dsi, frame_type, time_inc, size, start, is_coded);
	}
}