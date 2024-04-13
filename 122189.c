R_API char *U(r_bin_java_get_fcn_name_from_bin)(RBinJavaObj * bin_obj, ut64 addr) {
	RBinJavaField *fm_type = r_bin_java_get_method_code_attribute_with_addr (bin_obj, addr);
	return fm_type && fm_type->name ? strdup (fm_type->name) : NULL;
}