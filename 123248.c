static int r_cmd_java_set_acc_flags (RCore *core, ut64 addr, ut16 num_acc_flag) {
	char cmd_buf [50];
	//const char * fmt = "wx %04x @ 0x%"PFMT64x;

	int res = false;
	//ut64 cur_offset = core->offset;
	num_acc_flag = R_BIN_JAVA_USHORT (((ut8*) &num_acc_flag), 0);
	res = r_core_write_at(core, addr, (const ut8 *)&num_acc_flag, 2);
	if (!res) {
		eprintf ("[X] r_cmd_java_set_acc_flags: Failed to write.\n");
		return res;
	}
	//snprintf (cmd_buf, 50, fmt, num_acc_flag, addr);
	//res = r_core_cmd0(core, cmd_buf);
	res = true;
	IFDBG r_cons_printf ("Executed cmd: %s == %d\n", cmd_buf, res);
	/*if (cur_offset != core->offset) {
		IFDBG eprintf ("Ooops, write advanced the cursor, moving it back.");
		r_core_seek (core, cur_offset-2, 1);
	}*/
	return res;
}