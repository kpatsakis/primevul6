static int r_cmd_java_print_field_num_name (RBinJavaObj *obj) {
	RList * the_list = r_bin_java_get_field_num_name (obj);
	char * str;
	RListIter *iter = NULL;
	r_list_foreach (the_list, iter, str) {
		r_cons_println (str);
	}
	r_list_free (the_list);
	return true;
}