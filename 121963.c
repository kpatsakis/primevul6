R_API ut64 r_bin_java_rtvp_annotations_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 0;
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaAnnotationsArray *annotation_array;
	if (!attr) {
		return size;
	}
	size += (6 + 1);
	r_list_foreach_safe (attr->info.rtvp_annotations_attr.parameter_annotations,
		iter, iter_tmp, annotation_array) {
		if (annotation_array) {
			size += r_bin_java_annotation_array_calc_size (
				annotation_array);
		}
	}
	return size;
}