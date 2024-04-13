static int r_cmd_java_handle_replace_cp_value_double (RCore *core, RBinJavaObj *obj, const char *cmd, ut16 idx, ut64 addr) {
	double value = cmd && *cmd ? strtod (cmd, NULL) : 0.0;
	int res = false;
	res = r_cmd_java_get_cp_bytes_and_write (core, obj, idx, addr, (ut8 *) &value, 8);
	return res;
}