R_API void r_bin_java_rti_annotations_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr && attr->type == R_BIN_JAVA_ATTR_TYPE_RUNTIME_INVISIBLE_ANNOTATION_ATTR) {
		r_list_free (attr->info.annotation_array.annotations);
		free (attr->metas);
		free (attr->name);
		free (attr);
	}
}