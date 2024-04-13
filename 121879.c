R_API ut64 r_bin_java_local_variable_table_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 0;
	// ut64 offset = 0;
	RListIter *iter;
	// RBinJavaLocalVariableAttribute *lvattr;
	if (!attr) {
		return 0LL;
	}
	size += 6;
	// attr->info.local_variable_table_attr.table_length = R_BIN_JAVA_USHORT (buffer, offset);
	size += 2;
	// r_list_foreach (attr->info.local_variable_table_attr.local_variable_table, iter, lvattr) {
	r_list_foreach_iter (attr->info.local_variable_table_attr.local_variable_table, iter) {
		// lvattr->start_pc = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// lvattr->length = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// lvattr->name_idx = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// lvattr->descriptor_idx = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// lvattr->index = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
	}
	return size;
}