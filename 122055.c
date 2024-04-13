R_API int r_bin_java_print_method_idx_summary(RBinJavaObj *bin_obj, ut32 idx) {
	int res = false;
	if (idx < r_list_length (bin_obj->methods_list)) {
		RBinJavaField *fm_type = r_list_get_n (bin_obj->methods_list, idx);
		r_bin_java_print_method_summary (fm_type);
		res = true;
	}
	return res;
}