static void vp9_frame_size_with_refs(GF_BitStream *bs, u8 refresh_frame_flags, u8 * ref_frame_idx, int * RefFrameWidth, int *RefFrameHeight,
	int *FrameWidth, int *FrameHeight, int *RenderWidth, int *RenderHeight, int *Sb64Cols, int *Sb64Rows)
{
	Bool found_ref;
	int i;
	for (i = 0; i < 3; i++) {
		found_ref = gf_bs_read_int_log(bs, 1, "found_ref");
		if (found_ref) {
			*FrameWidth  = RefFrameWidth [ref_frame_idx[i]];
			*FrameHeight = RefFrameHeight[ref_frame_idx[i]];
			break;
		}
	}
	if (found_ref == 0) {
		vp9_frame_size(bs, FrameWidth, FrameHeight, Sb64Cols, Sb64Rows);
	}
	else {
		vp9_compute_image_size(*FrameWidth, *FrameHeight, Sb64Cols, Sb64Rows);
	}

	vp9_render_size(bs, *FrameWidth, *FrameHeight, RenderWidth, RenderHeight);
}