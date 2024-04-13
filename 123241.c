static int r_cmd_java_handle_flags_str_at (RCore *core, const char *cmd) {

	int res = false;
	ut64 flag_value_addr = -1;
	ut32 flag_value = -1;
	const char f_type = cmd ? *r_cmd_java_consumetok (cmd, ' ', -1) : 0;
	const char *p = cmd ? cmd + 2: NULL;
	char * flags_str = NULL;

	IFDBG r_cons_printf ("r_cmd_java_handle_flags_str_at: ftype = 0x%02x, idx = %s\n", f_type, p);
	if (p) {
		flag_value = 0;
		ut64 cur_offset = core->offset;
		flag_value_addr = r_cmd_java_is_valid_input_num_value(core, p) ? r_cmd_java_get_input_num_value (core, p) : -1;
		r_io_read_at (core->io, flag_value_addr, (ut8 *) &flag_value, 2);
		IFDBG r_cons_printf ("r_cmd_java_handle_flags_str_at: read = 0x%04x\n", flag_value);
		if (cur_offset != core->offset) {
			r_core_seek (core, cur_offset - 2, 1);
		}
		flag_value = R_BIN_JAVA_USHORT (((ut8 *) &flag_value), 0);
	}

	if (p && f_type) {
		switch (f_type) {
		case 'm': flags_str = retrieve_method_access_string((ut16) flag_value); break;
		case 'f': flags_str = retrieve_field_access_string((ut16) flag_value); break;
		case 'c': flags_str = retrieve_class_method_access_string((ut16) flag_value); break;
		default: flags_str = NULL;
		}
	}

	if (flags_str) {
		switch (f_type) {
		case 'm': r_cons_printf ("Method Access Flags String: "); break;
		case 'f': r_cons_printf ("Field Access Flags String: "); break;
		case 'c': r_cons_printf ("Class Access Flags String: "); break;
		}
		r_cons_println (flags_str);
		free (flags_str);
		res = true;
	}
	if (res == false) {
		eprintf ("[-] r_cmd_java: incorrect syntax for the flags calculation.\n");
		r_cmd_java_print_cmd_help (JAVA_CMDS+FLAGS_STR_IDX);
		res = true;
	}
	return res;
}