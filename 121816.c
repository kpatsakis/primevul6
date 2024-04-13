R_API void U(r_bin_java_stack_frame_default_free)(void *s) {
	RBinJavaStackMapFrame *stack_frame = s;
	if (stack_frame) {
		free (stack_frame->metas);
		free (stack_frame);
	}
}