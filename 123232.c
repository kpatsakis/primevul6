static int r_cmd_java_handle_calc_flags (RCore *core, const char *cmd) {
	IFDBG r_cons_printf ("Function call made: %s\n", cmd);
	int res = false;

	switch (*(cmd)) {
		case 'f': return r_cmd_java_print_field_access_flags_value (cmd+2);
		case 'm': return r_cmd_java_print_method_access_flags_value (cmd+2);
		case 'c': return r_cmd_java_print_class_access_flags_value (cmd+2);
	}

	if ( *(cmd) == 'l') {
		const char *lcmd = *cmd+1 == ' '? cmd+2 : cmd+1;
		IFDBG eprintf ("Seeing %s and accepting %s\n", cmd, lcmd);
		switch (*(lcmd)) {
		case 'f':
		case 'm':
		case 'c': res = r_cmd_java_get_all_access_flags_value (lcmd); break;
		}
		// Just print them all out
		if (res == false) {
			r_cmd_java_get_all_access_flags_value ("c");
			r_cmd_java_get_all_access_flags_value ("m");
			res = r_cmd_java_get_all_access_flags_value ("f");
		}
	}
	if (res == false) {
		eprintf ("[-] r_cmd_java: incorrect syntax for the flags calculation.\n");
		r_cmd_java_print_cmd_help (JAVA_CMDS+CALC_FLAGS_IDX);
		res = true;
	}
	return res;
}