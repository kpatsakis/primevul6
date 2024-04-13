static void av1_load_previous(AV1State *state, u8 primary_ref_frame, s8 *ref_frame_idx)
{
	s8 prevFrame = ref_frame_idx[primary_ref_frame];
	if (prevFrame < 0) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AV1] load_previous: prevFrame reference index %d is invalid\n", prevFrame));
	}
	else {
		state->PrevGmParams = state->SavedGmParams[prevFrame];
		// load_loop_filter_params( prevFrame )
		// load_segmentation_params( prevFrame )
	}
}