R_API ut64 r_bin_java_local_variable_type_table_attr_calc_size(RBinJavaAttrInfo *attr) {
	// RBinJavaLocalVariableTypeAttribute *lvattr;
	RListIter *iter;
	ut64 size = 0;
	if (attr) {
		RList *list = attr->info.local_variable_type_table_attr.local_variable_table;
		size += 6;
		// attr->info.local_variable_type_table_attr.table_length = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// r_list_foreach (list, iter, lvattr) {
		r_list_foreach_iter (list, iter) {
			// lvattr->start_pc = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			// lvattr->length = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			// lvattr->name_idx = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			// lvattr->signature_idx = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			// lvattr->index = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
		}
	}
	return size;
}