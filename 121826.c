R_API int U(r_bin_java_is_method_private)(RBinJavaObj * bin_obj, ut64 addr) {
	return r_bin_java_is_fm_type_private (r_bin_java_get_method_code_attribute_with_addr (bin_obj, addr));
}