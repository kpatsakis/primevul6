static int r_cmd_java_print_json_definitions( RBinJavaObj *obj ) {
	DsoJsonObj *json_obj = r_bin_java_get_bin_obj_json (obj);
	char *str = dso_json_obj_to_str (json_obj);
	dso_json_obj_del (json_obj); // XXX memleak
	r_cons_println (str);
	return true;
}