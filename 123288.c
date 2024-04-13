static int r_cmd_java_resolve_cp_type (RBinJavaObj *obj, ut16 idx) {
	if (obj && idx){
		char * str = r_bin_java_resolve_cp_idx_type (obj, idx);
		r_cons_println (str);
		free (str);
	}
	return true;
}