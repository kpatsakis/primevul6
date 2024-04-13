R_API char *r_bin_java_get_method_definition(RBinJavaField *fm_type) {
	return r_bin_java_unmangle (fm_type->flags_str, fm_type->name, fm_type->descriptor);
}