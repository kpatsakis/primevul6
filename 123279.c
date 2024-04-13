static int r_cmd_java_handle_field_info (RCore *core, const char *cmd) {
	RAnal *anal = get_anal (core);
	RBinJavaObj *obj = (RBinJavaObj *) r_cmd_java_get_bin_obj (anal);
	IFDBG r_cons_printf ("Function call made: %s\n", cmd);
	ut16 idx = -1;

	if (!obj) {
		eprintf ("[-] r_cmd_java: no valid java bins found.\n");
		return true;
	} else if (!cmd || !*cmd) {
		eprintf ("[-] r_cmd_java: invalid command syntax.\n");
		r_cmd_java_print_cmd_help (JAVA_CMDS+FIELD_INFO_IDX);
		return true;
	}

	if (*(cmd) == 's' || *(cmd) == 'n') {
		idx = r_cmd_java_get_input_num_value (core, cmd+1);
	}

	switch (*(cmd)) {
	case 'c': return r_cmd_java_print_field_num_name (obj);
	case 's': return r_cmd_java_print_field_summary (obj, idx);
	case 'n': return r_cmd_java_print_field_name (obj, idx);
	}
	IFDBG r_cons_printf ("Command is (%s)\n", cmd);
	eprintf ("[-] r_cmd_java: invalid command syntax.\n");
	r_cmd_java_print_cmd_help (JAVA_CMDS+FIELD_INFO_IDX);
	return false;
}