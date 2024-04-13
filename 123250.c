static RList * cpfind_str (RCore *core, RBinJavaObj *obj, const char *cmd) {
	if (!cmd) {
		return r_list_new ();
	}
	IFDBG r_cons_printf ("Looking for str: %s (%d)\n", cmd, strlen (cmd));
	return r_bin_java_find_cp_const_by_val ( obj, (const ut8 *) cmd, strlen (cmd), R_BIN_JAVA_CP_UTF8);
}