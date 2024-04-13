R_API ut64 r_bin_java_code_attr_calc_size(RBinJavaAttrInfo *attr) {
	RListIter *iter;
	// RListIter *iter_tmp;
	ut64 size = 0;
	if (attr) {
		// attr = r_bin_java_default_attr_new (buffer, sz, buf_offset);
		size += 6;
		// attr->info.code_attr.max_stack = R_BIN_JAVA_USHORT (buffer, 0);
		size += 2;
		// attr->info.code_attr.max_locals = R_BIN_JAVA_USHORT (buffer, 2);
		size += 2;
		// attr->info.code_attr.code_length = R_BIN_JAVA_UINT (buffer, 4);
		size += 2;
		if (attr->info.code_attr.code) {
			size += attr->info.code_attr.code_length;
		}
		// attr->info.code_attr.exception_table_length =  R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// RBinJavaExceptionEntry *exc_entry;
		// r_list_foreach_safe (attr->info.code_attr.exception_table, iter, iter_tmp, exc_entry) {
		r_list_foreach_iter (attr->info.code_attr.exception_table, iter) {
			// exc_entry->start_pc = R_BIN_JAVA_USHORT (buffer,offset);
			size += 2;
			// exc_entry->end_pc = R_BIN_JAVA_USHORT (buffer,offset);
			size += 2;
			// exc_entry->handler_pc = R_BIN_JAVA_USHORT (buffer,offset);
			size += 2;
			// exc_entry->catch_type = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
		}
		// attr->info.code_attr.attributes_count = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		// RBinJavaAttrInfo *_attr;
		if (attr->info.code_attr.attributes_count > 0) {
			// r_list_foreach_safe (attr->info.code_attr.attributes, iter, iter_tmp, _attr) {
			r_list_foreach_iter (attr->info.code_attr.attributes, iter) {
				size += r_bin_java_attr_calc_size (attr);
			}
		}
	}
	return size;
}