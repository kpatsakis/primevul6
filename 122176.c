R_API ut64 r_bin_java_inner_class_attr_calc_size(RBinJavaClassesAttribute *icattr) {
	ut64 size = 0;
	if (icattr) {
		// icattr->inner_class_info_idx = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// icattr->outer_class_info_idx = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// icattr->inner_name_idx = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// icattr->inner_class_access_flags = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
	}
	return size;
}