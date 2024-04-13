R_API void r_bin_java_rtvp_annotations_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		if (attr->type == R_BIN_JAVA_ATTR_TYPE_RUNTIME_VISIBLE_PARAMETER_ANNOTATION_ATTR) {
			r_list_free (attr->info.rtvp_annotations_attr.parameter_annotations);
		}
		free (attr->name);
		free (attr->metas);
		free (attr);
	}
}