static int r_cmd_java_handle_reload_bin (RCore *core, const char *cmd) {
	RAnal *anal = get_anal (core);
	RBinJavaObj *obj = (RBinJavaObj *) r_cmd_java_get_bin_obj (anal);
	const char *p = cmd;
	ut64 addr = 0LL; //cur_offset = core->offset, addr = 0;
	ut64 buf_size = 0;
	ut8 * buf = NULL;
	int res = false;

	if (*cmd == ' ') {
		p = r_cmd_java_consumetok (p, ' ', -1);
	}
	if (!*cmd) {
		r_cmd_java_print_cmd_help (JAVA_CMDS+RELOAD_BIN_IDX);
		return true;
	}

	addr = r_cmd_java_is_valid_input_num_value(core, p) ? r_cmd_java_get_input_num_value (core, p) : -1;
	if (*cmd == ' ') {
		p = r_cmd_java_consumetok (p, ' ', -1);
	}
	buf_size = r_cmd_java_is_valid_input_num_value(core, p) ? r_cmd_java_get_input_num_value (core, p) : -1;

	// XXX this may cause problems cause the file we are looking at may not be the bin we want.
	// lets pretend it is for now
	if (buf_size == 0) {
		res = r_io_use_fd (core->io, core->file->fd);
		buf_size = r_io_size (core->io);
		buf = malloc (buf_size);
		memset (buf, 0, buf_size);
		r_io_read_at (core->io, addr, buf, buf_size);
	}
	if (buf && obj) {
		res = r_cmd_java_reload_bin_from_buf (core, obj, buf, buf_size);
	}
	free (buf);
	return res;
}