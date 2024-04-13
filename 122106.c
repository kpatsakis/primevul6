R_API RBinJavaAnnotationsArray *r_bin_java_annotation_array_new(ut8 *buffer, ut64 sz, ut64 buf_offset) {
	RBinJavaAnnotation *annotation;
	RBinJavaAnnotationsArray *annotation_array;
	ut32 i;
	ut64 offset = 0;
	annotation_array = (RBinJavaAnnotationsArray *) malloc (sizeof (RBinJavaAnnotationsArray));
	if (annotation_array == NULL) {
		// TODO eprintf
		return NULL;
	}
	annotation_array->num_annotations = R_BIN_JAVA_USHORT (buffer, offset);
	offset += 2;
	annotation_array->annotations = r_list_new ();
	for (i = 0; i < annotation_array->num_annotations; i++) {
		annotation = r_bin_java_annotation_new (buffer + offset, sz - offset, buf_offset + offset);
		if (annotation) {
			offset += annotation->size;
		}
		if (annotation == NULL) {
			// TODO eprintf
		}
		r_list_append (annotation_array->annotations, (void *) annotation);
	}
	annotation_array->size = offset;
	return annotation_array;
}