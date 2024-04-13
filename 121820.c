R_API void r_bin_java_synthetic_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->metas);
		free (attr);
	}
}