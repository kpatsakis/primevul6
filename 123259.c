static int r_cmd_java_handle_replace_cp_value_float (RCore *core, RBinJavaObj *obj, const char *cmd, ut16 idx, ut64 addr) {
	float value = cmd && *cmd ? atof (cmd) : 0.0;
	int res = false;
	res = r_cmd_java_get_cp_bytes_and_write (core, obj, idx, addr, (ut8 *) &value, 4);
	return res;
}