R_API void r_bin_java_inner_classes_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		free (attr->name);
		free (attr->metas);
		r_list_free (attr->info.inner_classes_attr.classes);
		free (attr);
	}
}