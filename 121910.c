R_API RList *r_bin_java_get_method_exception_table_with_addr(RBinJavaObj *bin, ut64 addr) {
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaField *fm_type, *res = NULL;
	if (!bin && R_BIN_JAVA_GLOBAL_BIN) {
		bin = R_BIN_JAVA_GLOBAL_BIN;
	}
	if (!bin) {
		eprintf ("Attempting to analyse function when the R_BIN_JAVA_GLOBAL_BIN has not been set.\n");
		return NULL;
	}
	r_list_foreach_safe (bin->methods_list, iter, iter_tmp, fm_type) {
		ut64 offset = r_bin_java_get_method_code_offset (fm_type) + bin->loadaddr,
		size = r_bin_java_get_method_code_size (fm_type);
		if (addr >= offset && addr <= size + offset) {
			res = fm_type;
		}
	}
	if (res) {
		RBinJavaAttrInfo *code_attr = r_bin_java_get_method_code_attribute (res);
		return code_attr->info.code_attr.exception_table;
	}
	return NULL;
}