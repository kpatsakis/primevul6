R_API int U(r_bin_java_is_method_static)(RBinJavaObj * bin_obj, ut64 addr) {
	RBinJavaField *fm_type = r_bin_java_get_method_code_attribute_with_addr (bin_obj, addr);
	return fm_type && fm_type->flags & R_BIN_JAVA_METHOD_ACC_STATIC;
}