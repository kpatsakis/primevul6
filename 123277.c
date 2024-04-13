static int r_cmd_java_print_method_summary (RBinJavaObj *obj, ut16 idx) {
	int res = r_bin_java_print_method_idx_summary (obj, idx);
	if (res == false) {
		eprintf ("Error: Field or Method @ index (%d) not found in the RBinJavaObj.\n", idx);
		res = true;
	}
	return res;
}