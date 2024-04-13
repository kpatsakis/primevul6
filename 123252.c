static int r_cmd_java_handle_replace_cp_value_str (RCore *core, RBinJavaObj *obj, const char *cmd, ut16 idx, ut64 addr) {
	int res = false;
	ut32 len = cmd && *cmd ? strlen (cmd) : 0;
	if (len > 0 && cmd && *cmd == '"') {
		cmd++;
		len = cmd && *cmd ? strlen (cmd) : 0;
	}
	if (cmd && len > 0) {
		res = r_cmd_java_get_cp_bytes_and_write (core, obj, idx, addr, (ut8 *) cmd, len);
	}
	return res;
}