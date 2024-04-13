R_API ut64 r_bin_java_enclosing_methods_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 0;
	if (attr) {
		size += 6;
		// attr->info.enclosing_method_attr.class_idx = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// attr->info.enclosing_method_attr.method_idx = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
	}
	return size;
}