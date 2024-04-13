R_API void r_bin_java_fmtype_free(void /*RBinJavaField*/ *f) {
	RBinJavaField *fm_type = f;
	if (!fm_type) {
		return;
	}
	free (fm_type->descriptor);
	free (fm_type->name);
	free (fm_type->flags_str);
	free (fm_type->class_name);
	free (fm_type->metas);
	r_list_free (fm_type->attributes);
	free (fm_type);
}