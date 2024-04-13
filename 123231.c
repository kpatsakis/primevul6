static int r_cmd_java_handle_set_flags (RCore * core, const char * input) {
	//#define SET_ACC_FLAGS_ARGS "< c | m | f> <addr> <d | <s <flag value separated by space> >"
	const char *p = r_cmd_java_consumetok (input, ' ', -1);
	ut64 addr = p && r_cmd_java_is_valid_input_num_value (core, p)
		? r_cmd_java_get_input_num_value (core, p) : -1;
	p = r_cmd_java_strtok (p + 1, ' ', -1);
	if (!p || !*p) {
		r_cmd_java_print_cmd_help (JAVA_CMDS+SET_ACC_FLAGS_IDX);
		return true;
	}
	const char f_type = p && *p ? r_cmd_java_is_valid_java_mcf (*(++p)) : '?';

	int flag_value = r_cmd_java_is_valid_input_num_value(core, p) ? r_cmd_java_get_input_num_value (core, p) : -1;

	if (flag_value == 16 && f_type == 'f') {
		flag_value = -1;
	}
	IFDBG r_cons_printf ("Converting %s to flags\n",p);

	if (p) {
		p += 2;
	}
	if (flag_value == -1) {
		flag_value = r_cmd_java_is_valid_input_num_value (core, p) ? r_cmd_java_get_input_num_value (core, p) : -1;
	}
	bool res = false;
	if (!input) {
		eprintf ("[-] r_cmd_java: no address provided .\n");
		res = true;
	} else if (addr == -1) {
		eprintf ("[-] r_cmd_java: no address provided .\n");
		res = true;
	} else if (f_type == '?' && flag_value == -1) {
		eprintf ("[-] r_cmd_java: no flag type provided .\n");
		res = true;
	}

	if (res) {
		r_cmd_java_print_cmd_help (JAVA_CMDS + SET_ACC_FLAGS_IDX);
		return res;
	}

	IFDBG r_cons_printf ("Writing ftype '%c' to 0x%"PFMT64x", %s.\n", f_type, addr, p);

	// handling string based access flags (otherwise skip ahead)
	IFDBG r_cons_printf ("Converting %s to flags\n",p);
	if (f_type && flag_value != -1) {
		switch (f_type) {
		case 'f': flag_value = r_bin_java_calculate_field_access_value (p); break;
		case 'm': flag_value = r_bin_java_calculate_method_access_value (p); break;
		case 'c': flag_value = r_bin_java_calculate_class_access_value (p); break;
		default: flag_value = -1;
		}
	}
	IFDBG r_cons_printf ("Current args: (flag_value: 0x%04x addr: 0x%"PFMT64x")\n.", flag_value, addr, res);
	if (flag_value != -1) {
		res = r_cmd_java_set_acc_flags (core, addr, ((ut16) flag_value) & 0xffff);
		IFDBG r_cons_printf ("Writing 0x%04x to 0x%"PFMT64x": %d.", flag_value, addr, res);
	} else {
		eprintf ("[-] r_cmd_java: invalid flag value or type provided .\n");
		r_cmd_java_print_cmd_help (JAVA_CMDS + SET_ACC_FLAGS_IDX);
		res = true;
	}
	return res;
}