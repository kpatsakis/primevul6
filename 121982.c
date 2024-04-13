R_API DsoJsonObj *r_bin_java_get_import_json_definitions(RBinJavaObj *bin) {
	RList *the_list;
	DsoJsonObj *json_list = dso_json_list_new ();
	RListIter *iter = NULL;
	char *new_str;

	if (!bin || !(the_list = r_bin_java_get_lib_names (bin))) {
		return json_list;
	}

	r_list_foreach (the_list, iter, new_str) {
		char *tmp = new_str;
		// eprintf ("Processing string: %s\n", new_str);
		while (*tmp) {
			if (*tmp == '/') {
				*tmp = '.';
			}
			tmp++;
		}
		// eprintf ("adding string: %s\n", new_str);
		dso_json_list_append_str (json_list, new_str);
	}
	r_list_free (the_list);
	return json_list;
}