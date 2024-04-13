static int r_cmd_java_handle_replace_cp_value_long (RCore *core, RBinJavaObj *obj, const char *cmd, ut16 idx, ut64 addr) {
	ut64 value = r_cmd_java_get_input_num_value (core, cmd);
	int res = false;
	res = r_cmd_java_get_cp_bytes_and_write (core, obj, idx, addr, (ut8 *) &value, 8);
	return res;
}