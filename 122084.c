R_API void r_bin_java_rtip_annotations_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) { // && attr->type == R_BIN_JAVA_ATTR_TYPE_RUNTIME_INVISIBLE_PARAMETER_ANNOTATION_ATTR) {
		r_list_free (attr->info.rtip_annotations_attr.parameter_annotations);
		free (attr->metas);
		free (attr->name);
		free (attr);
	}
}