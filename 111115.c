static void vp9_render_size(GF_BitStream *bs, int FrameWidth, int FrameHeight, int *renderWidth, int *renderHeight)
{
	Bool render_and_frame_size_different = gf_bs_read_int_log(bs, 1, "render_and_frame_size_different");
	if (render_and_frame_size_different == 1) {
		int render_width_minus_1 = gf_bs_read_int_log(bs, 16, "render_width_minus_1");
		int render_height_minus_1 = gf_bs_read_int_log(bs, 16, "render_height_minus_1");
		*renderWidth = render_width_minus_1 + 1;
		*renderHeight = render_height_minus_1 + 1;
	}
	else {
		*renderWidth = FrameWidth;
		*renderHeight = FrameHeight;
	}
}