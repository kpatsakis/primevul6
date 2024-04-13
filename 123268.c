static int r_cmd_java_handle_prototypes (RCore *core, const char *cmd) {
	RAnal *anal = get_anal (core);
	RBinJavaObj *obj = (RBinJavaObj *) r_cmd_java_get_bin_obj (anal);
	IFDBG r_cons_printf ("Function call made: %s\n", cmd);

	if (!obj) {
		eprintf ("[-] r_cmd_java: no valid java bins found.\n");
		return true;
	}

	switch (*(cmd)) {
	case 'm': return r_cmd_java_print_method_definitions (obj);
	case 'f': return r_cmd_java_print_field_definitions (obj);
	case 'i': return r_cmd_java_print_import_definitions (obj);
	case 'c': return r_cmd_java_print_class_definitions (obj);
	case 'a': return r_cmd_java_print_all_definitions (anal);
	case 'j': return r_cmd_java_print_json_definitions (obj);
	}
	return false;
}