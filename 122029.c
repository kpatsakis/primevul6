R_API ut64 r_bin_java_get_method_end(RBinJavaObj *bin, RBinJavaField *fm_type) {
	return r_bin_java_get_method_code_offset (fm_type) + bin->loadaddr +
	+r_bin_java_get_method_code_size (fm_type);
}