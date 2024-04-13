R_API ut64 r_bin_java_exceptions_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 0, i = 0;
	if (attr) {
		size += 6;
		for (i = 0; i < attr->info.exceptions_attr.number_of_exceptions; i++) {
			// attr->info.exceptions_attr.exception_idx_table[i] = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
		}
	}
	return size;
}