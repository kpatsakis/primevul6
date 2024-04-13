R_API RBinJavaAttrInfo *r_bin_java_rtvp_annotations_attr_new(ut8 *buffer, ut64 sz, ut64 buf_offset) {
	ut32 i = 0;
	RBinJavaAttrInfo *attr = NULL;
	ut64 offset = 0;
	attr = r_bin_java_default_attr_new (buffer, sz, buf_offset);
	offset += 6;
	RBinJavaAnnotationsArray *annotation_array;
	if (attr) {
		attr->type = R_BIN_JAVA_ATTR_TYPE_RUNTIME_VISIBLE_PARAMETER_ANNOTATION_ATTR;
		attr->info.rtvp_annotations_attr.num_parameters = buffer[offset];
		offset += 1;
		attr->info.rtvp_annotations_attr.parameter_annotations = r_list_newf (r_bin_java_annotation_array_free);
		for (i = 0; i < attr->info.rtvp_annotations_attr.num_parameters; i++) {
			annotation_array = r_bin_java_annotation_array_new (buffer + offset, sz - offset, buf_offset + offset);
			if (annotation_array == NULL) {}
			if (annotation_array) {
				offset += annotation_array->size;
			}
			r_list_append (attr->info.rtvp_annotations_attr.parameter_annotations, (void *) annotation_array);
		}
		attr->size = offset;
	}
	return attr;
}