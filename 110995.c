static void av1_render_size(GF_BitStream *bs)
{
	Bool render_and_frame_size_different = gf_bs_read_int_log(bs, 1, "render_and_frame_size_different_flag");
	if (render_and_frame_size_different == GF_TRUE) {
		gf_bs_read_int_log(bs, 16, "render_width_minus_1");
		gf_bs_read_int_log(bs, 16, "render_height_minus_1");
		//RenderWidth = render_width_minus_1 + 1;
		//RenderHeight = render_height_minus_1 + 1;
	}
	else {
		//RenderWidth = UpscaledWidth;
		//RenderHeight = FrameHeight;
	}
}