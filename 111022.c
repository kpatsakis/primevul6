static void vp9_frame_size(GF_BitStream *bs, int *FrameWidth, int *FrameHeight, int *Sb64Cols, int *Sb64Rows)
{
	int frame_width_minus_1 = gf_bs_read_int_log(bs, 16, "frame_width_minus_1");
	int frame_height_minus_1 = gf_bs_read_int_log(bs, 16, "frame_height_minus_1");
	if (frame_width_minus_1 + 1 != *FrameWidth || frame_height_minus_1 + 1 != *FrameHeight) {
		if (*FrameWidth || *FrameHeight)
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[VP9] inconsistent frame dimensions: previous was %dx%d, new one is %dx%d.\n", *FrameWidth, *FrameHeight, frame_width_minus_1 + 1, frame_height_minus_1 + 1));
	}
	*FrameWidth = frame_width_minus_1 + 1;
	*FrameHeight = frame_height_minus_1 + 1;
	vp9_compute_image_size(*FrameWidth, *FrameHeight, Sb64Cols, Sb64Rows);
}