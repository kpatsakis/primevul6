R_API void r_bin_java_print_annotation_array_summary(RBinJavaAnnotationsArray *annotation_array) {
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaAnnotation *annotation;
	if (annotation_array->annotations == NULL) {
		// TODO eprintf
		return;
	}
	eprintf ("   Annotation Array Information:\n");
	eprintf ("   Number of Annotation Array Elements: %d\n", annotation_array->num_annotations);
	r_list_foreach_safe (annotation_array->annotations, iter, iter_tmp, annotation) {
		r_bin_java_print_annotation_summary (annotation);
	}
}