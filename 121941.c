R_API void r_bin_java_signature_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->metas);
		free (attr->info.signature_attr.signature);
		free (attr);
	}
}