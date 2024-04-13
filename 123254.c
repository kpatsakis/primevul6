static int r_cmd_java_get_cp_bytes_and_write (RCore *core, RBinJavaObj *obj, ut16 idx, ut64 addr, const ut8 * buf, const ut64 len) {
	int res = false;
	RBinJavaCPTypeObj *cp_obj = r_bin_java_get_item_from_bin_cp_list (obj, idx);
	ut64 c_file_sz = r_io_size (core->io);
	ut32 n_sz = 0, c_sz = obj ? r_bin_java_cp_get_size (obj, idx): -1;
	ut8 * bytes = NULL;

	if (c_sz == -1) {
		return res;
	}

	bytes = r_bin_java_cp_get_bytes (cp_obj->tag, &n_sz, buf, len);

	if (n_sz < c_sz) {
		res = r_core_shift_block (core, addr+c_sz, 0, (int)n_sz - (int)c_sz) &&
		r_io_resize(core->io, c_file_sz + (int) n_sz - (int) c_sz);
	} else if (n_sz > c_sz) {
		res = r_core_extend_at(core, addr,  (int)n_sz - (int)c_sz);
	} else {
		eprintf ("[X] r_cmd_java_get_cp_bytes_and_write: Failed to resize the file correctly aborting.\n");
		return res;
	}

	if (n_sz > 0 && bytes) {
		res = r_core_write_at(core, addr, (const ut8 *)bytes, n_sz) && r_core_seek (core, addr, 1);
	}

	if (res == false) {
		eprintf ("[X] r_cmd_java_get_cp_bytes_and_write: Failed to write the bytes to the file correctly aborting.\n");
		return res;
	}

	R_FREE (bytes);

	if (res == true) {
		ut64 n_file_sz = 0;
		ut8 * bin_buffer = NULL;
		res = r_io_use_fd (core->io, core->file->fd);
		n_file_sz = r_io_size (core->io);
		bin_buffer = n_file_sz > 0 ? malloc (n_file_sz) : NULL;
		if (bin_buffer) {
			memset (bin_buffer, 0, n_file_sz);
			res = n_file_sz == r_io_read_at (core->io, obj->loadaddr,
				bin_buffer, n_file_sz) ? true : false;
			if (res == true) {
				res = r_cmd_java_reload_bin_from_buf (
					core, obj, bin_buffer, n_file_sz);
			} else {
				eprintf ("[X] r_cmd_java_get_cp_bytes_and_write: Failed to read the file in aborted, bin reload.\n");
			}
			free (bin_buffer);
		}
	}
	return res;
}