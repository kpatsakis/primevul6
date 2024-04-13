R_API void r_bin_java_exceptions_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->metas);
		free (attr->info.exceptions_attr.exception_idx_table);
		free (attr);
	}
}