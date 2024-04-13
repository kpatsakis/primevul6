static RList * cpfind_double (RCore *core, RBinJavaObj *obj, const char *cmd) {
	double value = cmd && *cmd ? strtod (cmd, NULL) : 0.0;
	if (value == 0.0 && !(cmd && cmd[0] == '0' && cmd[1] == '.' && cmd[2] == '0')) {
		return r_list_new ();
	}
	return r_bin_java_find_cp_const_by_val ( obj, (const ut8 *) &value, 8, R_BIN_JAVA_CP_DOUBLE);
}