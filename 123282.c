static int r_cmd_java_handle_replace_cp_value_int (RCore *core, RBinJavaObj *obj, const char *cmd, ut16 idx, ut64 addr) {
	ut32 value = (ut32) r_cmd_java_get_input_num_value (core, cmd);
	int res = false;
	res = r_cmd_java_get_cp_bytes_and_write (core, obj, idx, addr, (ut8 *) &value, 4);
	return res;
}