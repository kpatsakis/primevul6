R_API void r_bin_java_print_source_code_file_attr_summary(RBinJavaAttrInfo *attr) {
	if (!attr) {
		eprintf ("Attempting to print an invalid RBinJavaAttrInfo *SourceFile.\n");
		return;
	}
	Eprintf ("Source File Attribute Information:\n");
	Eprintf ("  Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
	Eprintf ("  Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
	Eprintf ("  Attribute Length: %d\n", attr->length);
	Eprintf ("  Source File Index: %d\n", attr->info.source_file_attr.sourcefile_idx);
}