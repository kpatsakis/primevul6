static int r_cmd_java_handle_resolve_cp (RCore *core, const char *cmd) {
	RAnal *anal = get_anal (core);
	char c_type = cmd && *cmd ? *cmd : 0;
	RBinJavaObj *obj = r_cmd_java_get_bin_obj (anal);
	ut16 idx = r_cmd_java_get_input_num_value (core, cmd+2);
	IFDBG r_cons_printf ("Function call made: %s\n", cmd);
	IFDBG r_cons_printf ("Ctype: %d (%c) RBinJavaObj points to: %p and the idx is (%s): %d\n", c_type, c_type, obj, cmd+2, idx);
	int res = false;
	if (idx > 0 && obj) {
		switch (c_type) {
		case 't': return r_cmd_java_resolve_cp_type (obj, idx);
		case 'c': return r_cmd_java_resolve_cp_idx (obj, idx);
		case 'e': return r_cmd_java_resolve_cp_idx_b64 (obj, idx);
		case 'a': return r_cmd_java_resolve_cp_address (obj, idx);
		case 's': return r_cmd_java_resolve_cp_summary (obj, idx);
		case 'k': return r_cmd_java_resolve_cp_to_key (obj, idx);
		}
	} else if (obj && c_type == 'g') {
		for (idx = 1; idx <=obj->cp_count; idx++) {
			ut64 addr = r_bin_java_resolve_cp_idx_address (obj, idx) ;
			char * str = r_bin_java_resolve_cp_idx_type (obj, idx);
			r_cons_printf ("CP_OBJ Type %d =  %s @ 0x%"PFMT64x"\n", idx, str, addr);
			free (str);
		}
		res = true;
	} else if (obj && c_type == 'd') {
		for (idx = 1; idx <= obj->cp_count; idx++) {
			r_cmd_java_resolve_cp_summary (obj, idx);
		}
		res = true;
	} else {
		if (!obj) {
			eprintf ("[-] r_cmd_java: no valid java bins found.\n");
		} else {
			eprintf ("[-] r_cmd_java: invalid cp index given, must idx > 1.\n");
			r_cmd_java_print_cmd_help (JAVA_CMDS+RESOLVE_CP_IDX);
		}
		res = true;
	}
	return res;
}