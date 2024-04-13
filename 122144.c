R_API int r_bin_java_print_field_idx_summary(RBinJavaObj *bin_obj, ut32 idx) {
	int res = false;
	if (idx < r_list_length (bin_obj->fields_list)) {
		RBinJavaField *fm_type = r_list_get_n (bin_obj->fields_list, idx);
		r_bin_java_print_field_summary (fm_type);
		res = true;
	}
	return res;
}