R_API void r_bin_java_code_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		// XXX - Intentional memory leak here.  When one of the
		// Code attributes is parsed, the code (the r_bin_java)
		// is not properly parsing the class file
		r_bin_java_stack_frame_free (attr->info.code_attr.implicit_frame);
		r_list_free (attr->info.code_attr.attributes);
		free (attr->info.code_attr.code);
		r_list_free (attr->info.code_attr.exception_table);
		free (attr->name);
		free (attr->metas);
		free (attr);
	}
}