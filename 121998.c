R_API void r_bin_java_print_rtvp_annotations_attr_summary(RBinJavaAttrInfo *attr) {
	RBinJavaAnnotationsArray *annotation_array = NULL;
	RListIter *iter = NULL, *iter_tmp = NULL;
	if (attr && attr->type == R_BIN_JAVA_ATTR_TYPE_RUNTIME_VISIBLE_PARAMETER_ANNOTATION_ATTR) {
		eprintf ("Runtime Visible Parameter Annotations Attribute Information:\n");
		eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
		eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
		eprintf ("  Attribute Length: %d\n", attr->length);
		eprintf ("  Number of Runtime Invisible Parameters: %d\n", attr->info.rtvp_annotations_attr.num_parameters);
		r_list_foreach_safe (attr->info.rtvp_annotations_attr.parameter_annotations, iter, iter_tmp, annotation_array) {
			r_bin_java_print_annotation_array_summary (annotation_array);
		}
	}
}