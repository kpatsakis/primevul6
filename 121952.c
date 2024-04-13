R_API void r_bin_java_print_local_variable_type_table_attr_summary(RBinJavaAttrInfo *attr) {
	RBinJavaLocalVariableTypeAttribute *lvtattr;
	RListIter *iter, *iter_tmp;
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *LocalVariableTable.\n");
		return;
	}
	eprintf ("Local Variable Type Table Attribute Information:\n");
	eprintf ("   Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	eprintf ("   Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	eprintf ("   Attribute Length: %d\n", attr->length);
	r_list_foreach_safe (attr->info.local_variable_type_table_attr.local_variable_table, iter, iter_tmp, lvtattr) {
		r_bin_java_print_local_variable_type_attr_summary (lvtattr);
	}
}