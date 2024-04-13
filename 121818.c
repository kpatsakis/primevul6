R_API ut64 r_bin_java_stack_map_table_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 0;
	RListIter *iter, *iter_tmp;
	RBinJavaStackMapFrame *sf;
	if (attr) {
		// attr = r_bin_java_default_attr_new (buffer, sz, buf_offset);
		size += 6;
		// IFDBG r_bin_java_print_source_code_file_attr_summary(attr);
		// Current spec does not call for variable sizes.
		// attr->info.stack_map_table_attr.number_of_entries = R_BIN_JAVA_USHORT (buffer, offset);
		size += 2;
		r_list_foreach_safe (attr->info.stack_map_table_attr.stack_map_frame_entries, iter, iter_tmp, sf) {
			size += r_bin_java_stack_map_frame_calc_size (sf);
		}
	}
	return size;
}