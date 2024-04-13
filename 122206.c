R_API RBinJavaStackMapFrame *r_bin_java_default_stack_frame() {
	RBinJavaStackMapFrame *sf = R_NEW0 (RBinJavaStackMapFrame);
	if (!sf) {
		return NULL;
	}
	sf->metas = R_NEW0 (RBinJavaMetaInfo);
	if (!sf->metas) {
		free (sf);
		return NULL;
	}
	sf->metas->type_info = (void *) &R_BIN_JAVA_STACK_MAP_FRAME_METAS[R_BIN_JAVA_STACK_FRAME_IMPLICIT];
	sf->type = ((RBinJavaStackMapFrameMetas *) sf->metas->type_info)->type;
	sf->local_items = r_list_newf (r_bin_java_verification_info_free);
	sf->stack_items = r_list_newf (r_bin_java_verification_info_free);
	sf->number_of_stack_items = 0;
	sf->number_of_locals = 0;
	return sf;
}