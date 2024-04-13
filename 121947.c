R_API ut64 r_bin_java_inner_classes_attr_calc_size(RBinJavaAttrInfo *attr) {
	RBinJavaClassesAttribute *icattr = NULL;
	RListIter *iter;
	ut64 size = 6;
	if (!attr) {
		return 0;
	}
	r_list_foreach (attr->info.inner_classes_attr.classes, iter, icattr) {
		size += r_bin_java_inner_class_attr_calc_size (icattr);
	}
	return size;
}