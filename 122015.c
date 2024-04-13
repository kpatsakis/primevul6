R_API ut64 r_bin_java_source_debug_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 6;
	if (!attr) {
		return 0LL;
	}
	if (attr->info.debug_extensions.debug_extension) {
		size += attr->length;
	}
	return size;
}