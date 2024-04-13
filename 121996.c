R_API ut64 r_bin_java_annotation_array_calc_size(RBinJavaAnnotationsArray *annotation_array) {
	ut64 size = 0;
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaAnnotation *annotation;
	if (annotation_array->annotations == NULL) {
		// TODO eprintf
		return size;
	}
	// annotation_array->num_annotations = R_BIN_JAVA_USHORT (buffer, offset);
	size += 2;
	r_list_foreach_safe (annotation_array->annotations, iter, iter_tmp, annotation) {
		size += r_bin_java_annotation_calc_size (annotation);
	}
	return size;
}