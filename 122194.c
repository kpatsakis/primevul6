R_API void r_bin_java_print_annotation_default_attr_summary(RBinJavaAttrInfo *attr) {
	if (attr && attr->type == R_BIN_JAVA_ATTR_TYPE_ANNOTATION_DEFAULT_ATTR) {
		eprintf ("Annotation Default Attribute Information:\n");
		eprintf ("   Attribute Offset: 0x%08"PFMT64x "\n", attr->file_offset);
		eprintf ("   Attribute Name Index: %d (%s)\n", attr->name_idx, attr->name);
		eprintf ("   Attribute Length: %d\n", attr->length);
		r_bin_java_print_element_value_summary ((attr->info.annotation_default_attr.default_value));
	} else {
		// TODO: eprintf attr is invalid
	}
}