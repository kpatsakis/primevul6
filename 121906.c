R_API ut64 r_bin_java_line_number_table_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 6;
	// RBinJavaLineNumberAttribute *lnattr;
	RListIter *iter;
	// RListIter *iter_tmp;
	if (!attr) {
		return 0LL;
	}
	// r_list_foreach_safe (attr->info.line_number_table_attr.line_number_table, iter, iter_tmp, lnattr) {
	r_list_foreach_iter (attr->info.line_number_table_attr.line_number_table, iter) {
		// lnattr->start_pc = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// lnattr->line_number = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
	}
	return size;
}