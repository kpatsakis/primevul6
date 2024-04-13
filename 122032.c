R_API ut64 r_bin_java_find_method_offset(RBinJavaObj *bin, const char *method_name) {
	RListIter *attr_iter = NULL, *attr_iter_tmp = NULL;
	RBinJavaField *method = NULL;
	ut64 offset = -1;
	r_list_foreach_safe (bin->methods_list, attr_iter, attr_iter_tmp, method) {
		if (method && !strcmp ((const char *) method->name, method_name)) {
			offset = r_bin_java_get_method_code_offset (method) + bin->loadaddr;
			break;
		}
	}
	return offset;
}