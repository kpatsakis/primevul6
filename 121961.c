R_API ut64 r_bin_java_rtv_annotations_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 0;
	if (attr == NULL) {
		// TODO eprintf allocation fail
		return size;
	}
	size += (6 + r_bin_java_annotation_array_calc_size (&(attr->info.annotation_array)));
	return size;
}