R_API void r_bin_java_stack_frame_free(void /*RBinJavaStackMapFrame*/ *o) {
	RBinJavaStackMapFrame *obj = o;
	if (obj) {
		r_list_free (obj->local_items);
		r_list_free (obj->stack_items);
		free (obj->metas);
		free (obj);
	}
}