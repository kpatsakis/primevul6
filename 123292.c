static int r_cmd_java_handle_summary_info (RCore *core, const char *cmd) {
	RAnal *anal = get_anal (core);
	RBinJavaObj *obj = (RBinJavaObj *) r_cmd_java_get_bin_obj (anal);
	IFDBG r_cons_printf ("Function call made: %s\n", cmd);

	if (!obj) {
		eprintf ("[-] r_cmd_java: no valid java bins found.\n");
		return true;
	}

	r_cons_printf ("Summary for %s:\n", obj->file);
	r_cons_printf ("  Size 0x%"PFMT64x":\n", obj->size);
	r_cons_printf ("  Constants  size: 0x%"PFMT64x" count: %d:\n", obj->cp_size, obj->cp_count);
	r_cons_printf ("  Methods    size: 0x%"PFMT64x" count: %d:\n", obj->methods_size, obj->methods_count);
	r_cons_printf ("  Fields     size: 0x%"PFMT64x" count: %d:\n", obj->fields_size, obj->fields_count);
	r_cons_printf ("  Attributes size: 0x%"PFMT64x" count: %d:\n", obj->attrs_size, obj->attrs_count);
	r_cons_printf ("  Interfaces size: 0x%"PFMT64x" count: %d:\n", obj->interfaces_size, obj->interfaces_count);

	return true;
}