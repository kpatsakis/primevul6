R_API void r_bin_java_annotation_free(void /*RBinJavaAnnotation*/ *a) {
	RBinJavaAnnotation *annotation = a;
	if (annotation) {
		r_list_free (annotation->element_value_pairs);
		free (annotation);
	}
}