R_API void r_bin_java_source_debug_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->metas);
		free (attr->info.debug_extensions.debug_extension);
		free (attr);
	}
}