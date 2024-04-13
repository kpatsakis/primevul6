R_API RList *U(r_bin_java_get_args_from_bin)(RBinJavaObj * bin_obj, ut64 addr) {
	RBinJavaField *fm_type = r_bin_java_get_method_code_attribute_with_addr (bin_obj, addr);
	return fm_type ? r_bin_java_get_args (fm_type) : NULL;
}