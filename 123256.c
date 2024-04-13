static int r_cmd_java_print_field_name (RBinJavaObj *obj, ut16 idx) {
	char * res = r_bin_java_get_field_name (obj, idx);
	if (res) {
		r_cons_println (res);
	} else {
		eprintf ("Error: Field or Method @ index (%d) not found in the RBinJavaObj.\n", idx);
	}
	free (res);
	return true;
}