static int r_cmd_java_resolve_cp_summary (RBinJavaObj *obj, ut16 idx) {
	if (obj && idx){
		r_bin_java_resolve_cp_idx_print_summary (obj, idx) ;
	}
	return true;
}