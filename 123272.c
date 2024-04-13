static int r_cmd_java_handle_replace_classname_value (RCore *core, const char *cmd) {
	RBinJavaObj *obj;
	char *class_name = NULL, *new_class_name = NULL;
	ut32 class_name_len = 0, new_class_name_len = 0;
	RAnal *anal = get_anal (core);
	const char *p = cmd;
	int res = false;
	ut16 idx = -1;

	if (!core || !anal || !cmd) {
		return false;
	}
	IFDBG r_cons_printf ("Function call made: %s\n", p);
	obj = (RBinJavaObj *) r_cmd_java_get_bin_obj (anal);
	if (!obj) {
		eprintf ("The current binary is not a Java Bin Object.\n");
		return true;
	}
	res = r_cmd_java_get_class_names_from_input (cmd, &class_name,
		&class_name_len, &new_class_name, &new_class_name_len);

	if (!res || !class_name || !new_class_name) {
		r_cmd_java_print_cmd_help (JAVA_CMDS+REPLACE_CLASS_NAME_IDX);
		free (class_name);
		free (new_class_name);
		return true;
	}
	for (idx = 1; idx <=obj->cp_count; idx++) {
		RBinJavaCPTypeObj* cp_obj = r_bin_java_get_item_from_bin_cp_list (obj, idx);
		char *name = NULL;
		ut8 * buffer = NULL;
		ut32 buffer_sz = 0;
		ut16 len = 0;
		if (cp_obj && cp_obj->tag == R_BIN_JAVA_CP_UTF8 &&
			cp_obj->info.cp_utf8.length && cp_obj->info.cp_utf8.length >= class_name_len-1) {
			ut32 num_occurrences = 0;
			ut64 addr = cp_obj->file_offset + cp_obj->loadaddr;
			buffer = r_bin_java_cp_get_idx_bytes (obj, idx, &buffer_sz);

			if (!buffer) {
				continue;
			}
			len = R_BIN_JAVA_USHORT ( buffer, 1);
			name = malloc (len+3);
			memcpy (name, buffer+3, len);
			name[len] = 0;

			num_occurrences = r_cmd_get_num_classname_str_occ (name, class_name);

			if (num_occurrences > 0) {
				// perform inplace replacement
				ut32 res_len = 0;
				char * result = NULL;

				if (r_cmd_is_object_descriptor (name, len) == true) {
					result = r_cmd_replace_name_def (new_class_name,
						new_class_name_len-1, class_name,
						class_name_len-1, name, len, &res_len);
				} else {
					result = r_cmd_replace_name (new_class_name,
						new_class_name_len-1, class_name,
						class_name_len-1, name, len, &res_len);
				}
				if (result) {
					res = r_cmd_java_get_cp_bytes_and_write (
						core, obj, idx, addr,
						(const ut8*)result, res_len);
					if  (res == false) {
						eprintf ("ERROR: r_cmd_java: Failed to write bytes or reload the binary.\n");
					}
				}
				free (result);
			}
			free (buffer);
			free (name);
		}

	}
	free (class_name);
	free (new_class_name);
	return true;
}