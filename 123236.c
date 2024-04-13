static int r_cmd_java_get_all_access_flags_value (const char *cmd) {
	RList *the_list = NULL;
	RListIter *iter = NULL;
	char *str = NULL;

	switch (*(cmd)) {
	case 'f': the_list = retrieve_all_field_access_string_and_value (); break;
	case 'm': the_list = retrieve_all_method_access_string_and_value (); break;
	case 'c': the_list = retrieve_all_class_access_string_and_value (); break;
	}
	if (!the_list) {
		eprintf ("[-] r_cmd_java: incorrect syntax for the flags calculation.\n");
		r_cmd_java_print_cmd_help (JAVA_CMDS+CALC_FLAGS_IDX);
		return false;
	}
	switch (*(cmd)) {
	case 'f': r_cons_printf ("[=] Fields Access Flags List\n"); break;
	case 'm': r_cons_printf ("[=] Methods Access Flags List\n"); break;
	case 'c': r_cons_printf ("[=] Class Access Flags List\n");; break;
	}

	r_list_foreach (the_list, iter, str) {
		r_cons_println (str);
	}
	r_list_free (the_list);
	return true;
}