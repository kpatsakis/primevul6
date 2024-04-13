static int r_cmd_java_handle_replace_cp_value (RCore *core, const char *cmd) {
	RBinJavaObj *obj = (RBinJavaObj *) r_cmd_java_get_bin_obj (get_anal (core));
	ut16 idx = -1;
	ut64 addr = 0;
	const char *p = cmd;
	char cp_type = 0;
	IFDBG r_cons_printf ("Function call made: %s\n", p);
	if (p && *p) {
		p = r_cmd_java_consumetok (cmd, ' ', -1);
		if (r_cmd_java_is_valid_input_num_value (core, p)) {
			idx = r_cmd_java_get_input_num_value (core, p);
			p = r_cmd_java_strtok (p, ' ', strlen(p));
		}
	}
	if (idx == (ut16) -1 ) {
		eprintf ("[-] r_cmd_java: Invalid index value.\n");
	} else if (!obj) {
		eprintf ("[-] r_cmd_java: The current binary is not a Java Bin Object.\n");
	} else if (!p || (p && !*p)) {
		r_cmd_java_print_cmd_help (JAVA_CMDS+REPLACE_CP_VALUE_IDX);
		return true;
	} else {
		cp_type = r_bin_java_resolve_cp_idx_tag(obj, idx);
		addr = r_bin_java_resolve_cp_idx_address (obj, idx);
		IFDBG r_cons_printf ("Function call made: %s\n", p);
		switch (cp_type) {
		case R_BIN_JAVA_CP_UTF8: return r_cmd_java_handle_replace_cp_value_str (
			core, obj, r_cmd_java_consumetok (p, ' ', -1), idx, addr);
		case R_BIN_JAVA_CP_INTEGER: return r_cmd_java_handle_replace_cp_value_int (
			core, obj, r_cmd_java_consumetok (p, ' ', -1), idx, addr);
		case R_BIN_JAVA_CP_LONG: return r_cmd_java_handle_replace_cp_value_long (
			core, obj, r_cmd_java_consumetok (p, ' ', -1), idx, addr);
		case R_BIN_JAVA_CP_FLOAT: return r_cmd_java_handle_replace_cp_value_float (
			core, obj, r_cmd_java_consumetok (p, ' ', -1), idx, addr);
		case R_BIN_JAVA_CP_DOUBLE: return r_cmd_java_handle_replace_cp_value_double (
			core, obj, r_cmd_java_consumetok (p, ' ', -1), idx, addr);
		default:
			eprintf ("[-] r_cmd_java: invalid java type to search for.\n");
			return false;
		}
		return true;
	}
	return false;
}