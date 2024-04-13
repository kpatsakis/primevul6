static int r_cmd_java_handle_calc_class_sz (RCore *core, const char *cmd) {
	int res = false;
	ut64 sz = UT64_MAX;
	ut64 addr = UT64_MAX;
	ut64 res_size = UT64_MAX,
	cur_fsz = r_io_fd_size (core->io, r_core_file_cur (core)->fd);
	ut8 *tbuf, *buf = NULL;
	ut32 init_size = (1 << 16);
	const char *p = cmd ? r_cmd_java_consumetok (cmd, ' ', -1): NULL;
	addr = p && *p && r_cmd_java_is_valid_input_num_value(core, p) ? r_cmd_java_get_input_num_value (core, p) : UT64_MAX;

	// TODO add a size parameter to the command to skip the guessing part.

	if (addr != UT64_MAX && sz == UT64_MAX) {
		IFDBG r_cons_printf ("Function call made: %s\n", cmd);
		IFDBG r_cons_printf ("Attempting to calculate class file size @ : 0x%"PFMT64x".\n", addr);
		sz = cur_fsz < init_size ? cur_fsz : init_size;
		while (sz <= cur_fsz) {
			tbuf = realloc (buf, sz);
			if (!tbuf) {
				eprintf ("Memory allocation failed.\n");
				free (buf);
				break;
			}
			buf = tbuf;
			ut64 r_sz = r_io_read_at (core->io, addr, buf, sz) ? sz : 0LL;
			// check the return read on the read
			if (r_sz == 0) {
				break;
			}
			res_size = r_bin_java_calc_class_size (buf, sz);
			// if the data buffer contains a class starting
			// at address, then the res_size will be the size
			// if the r_sz is less than the sz, then we are near
			// the end of the core buffer, and there is no need
			// to continue trying to find the class size.
			if (res_size != UT64_MAX ||
				r_sz < sz) {
				res = r_sz < sz ? false : true;
				free (buf);
				break;
			}else {
				sz += (1 << 16);
			}
		}
		if (res) {
			r_cons_printf ("%" PFMT64d, res_size);
		} else {
			r_cons_printf ("-1\n");
		}

		//snprintf (cmd_buf, 50, fmt, num_acc_flag, addr);
		//res = r_core_cmd0(core, y);
	} else {
		r_cmd_java_print_cmd_help (JAVA_CMDS+CALC_SZ_IDX);
	}
	return true;
}