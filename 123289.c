static RList * cpfind_int (RCore *core, RBinJavaObj *obj, const char *cmd) {
	ut32 value = (ut32) r_cmd_java_get_input_num_value (core, cmd);
	if (!r_cmd_java_is_valid_input_num_value (core, cmd)) {
		return r_list_new ();
	}
	return r_bin_java_find_cp_const_by_val ( obj, (const ut8 *) &value, 4, R_BIN_JAVA_CP_INTEGER);
}