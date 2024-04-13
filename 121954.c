R_API void r_bin_java_print_classes_attr_summary(RBinJavaClassesAttribute *icattr) {
	if (!icattr) {
		eprintf ("Attempting to print an invalid RBinJavaClassesAttribute* (InnerClasses element).\n");
		return;
	}
	eprintf ("   Inner Classes Class Attribute Offset: 0x%08"PFMT64x "\n", icattr->file_offset);
	eprintf ("   Inner Classes Class Attribute Class Name (%d): %s\n", icattr->inner_name_idx, icattr->name);
	eprintf ("   Inner Classes Class Attribute Class inner_class_info_idx: %d\n", icattr->inner_class_info_idx);
	eprintf ("   Inner Classes Class Attribute Class inner_class_access_flags: 0x%02x %s\n", icattr->inner_class_access_flags, icattr->flags_str);
	eprintf ("   Inner Classes Class Attribute Class outer_class_info_idx: %d\n", icattr->outer_class_info_idx);
	eprintf ("   Inner Classes Class Field Information:\n");
	r_bin_java_print_field_summary (icattr->clint_field);
	eprintf ("   Inner Classes Class Field Information:\n");
	r_bin_java_print_field_summary (icattr->clint_field);
	eprintf ("   Inner Classes Class Attr Info Information:\n");
	r_bin_java_print_attr_summary (icattr->clint_attr);
}