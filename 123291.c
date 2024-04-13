static int cpfind (RCore *core, const char *cmd) {
	RBinJavaObj *obj = (RBinJavaObj *) r_cmd_java_get_bin_obj (get_anal (core));
	const char *p = cmd;
	char f_type = 0;
	RList *find_list = NULL;
	RListIter *iter;
	ut32 *idx;

	if (!obj) {
		eprintf ("[-] r_cmd_java: no valid java bins found.\n");
		return true;
	}
	IFDBG r_cons_printf ("Function call made: %s\n", p);
	if (p && *p) {
		p = r_cmd_java_consumetok (cmd, ' ', -1);
		f_type = *p;
		p+=2;
	}
	IFDBG r_cons_printf ("Function call made: %s\n", p);
	switch (f_type) {
	case 's': find_list = cpfind_str (core, obj, p); break;
	case 'i': find_list = cpfind_int (core, obj, r_cmd_java_consumetok (p, ' ', -1)); break;
	case 'l': find_list = cpfind_long (core, obj, r_cmd_java_consumetok (p, ' ', -1)); break;
	case 'f': find_list = cpfind_float (core, obj, r_cmd_java_consumetok (p, ' ', -1)); break;
	case 'd': find_list = cpfind_double (core, obj, r_cmd_java_consumetok (p, ' ', -1)); break;
	default:
		eprintf ("[-] r_cmd_java: invalid java type to search for.\n");
		return true;
	}

	r_list_foreach (find_list, iter, idx) {
		ut64 addr = r_bin_java_resolve_cp_idx_address (obj, (ut16) *idx);
		r_cons_printf ("Offset: 0x%"PFMT64x" idx: %d\n", addr, *idx);
	}
	r_list_free (find_list);
	return true;
}