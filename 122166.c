R_API ut64 r_bin_java_attr_calc_size(RBinJavaAttrInfo *attr) {
	return attr ? ((RBinJavaAttrMetas *) attr->metas->type_info)->allocs->calc_size (attr) : 0;
}