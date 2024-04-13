static int r_cmd_java_handle_yara_code_extraction_refs (RCore *core, const char *input) {
	RAnal *anal = get_anal (core);
	RBinJavaObj *bin = anal ? (RBinJavaObj *) r_cmd_java_get_bin_obj (anal) : NULL;
	const char *p = input? r_cmd_java_consumetok (input, ' ', -1): NULL, *n = NULL;
	char *name = NULL;
	ut64 addr = -1, count = -1;
	int res = false;

	if (!bin) {
		return res;
	} else if (!anal || !anal->fcns || r_list_length (anal->fcns) == 0) {
		eprintf ("Unable to access the current analysis, perform 'af' for function analysis.\n");
		return true;
	}

	if (!p) {
		return res;
	}

	n = *p ? r_cmd_java_strtok (p, ' ', -1) : NULL;
	name = n && p && p != n ? malloc (n - p + 2) : NULL;

	if (!name) {
		return res;
	}

	memset (name, 0, n-p);
	memcpy (name, p, n-p);

	p = r_cmd_java_strtok (p, ' ', -1);
	addr = p && *p && r_cmd_java_is_valid_input_num_value(core, p) ? r_cmd_java_get_input_num_value (core, p) : -1;

	p = r_cmd_java_strtok (p, ' ', -1);
	count = p && *p && r_cmd_java_is_valid_input_num_value(core, p) ? r_cmd_java_get_input_num_value (core, p) : -1;

	if (name && count != (ut64) -1 && addr != (ut64) -1) {
		// find function at addr

		// find the start basic block

		// read the bytes

		// hexlify the bytes

		// set the name = bytes

		// print t
	}
	free (name);
	return res;
}