R_API void r_bin_java_bootstrap_methods_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr && attr->type == R_BIN_JAVA_ATTR_TYPE_BOOTSTRAP_METHODS_ATTR) {
		free (attr->name);
		free (attr->metas);
		r_list_free (attr->info.bootstrap_methods_attr.bootstrap_methods);
		free (attr);
	}
}