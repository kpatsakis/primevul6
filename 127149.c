R_API int r_bin_dwarf_parse_info(RBinDwarfDebugAbbrev *da, RBin *a, int mode) {
	ut8 *buf, *debug_str_buf = 0;
	int len, debug_str_len = 0, ret;
	RBinSection *debug_str;
	RBinSection *section = getsection (a, "debug_info");
	RBinFile *binfile = a ? a->cur: NULL;

	if (binfile && section) {
		debug_str = getsection (a, "debug_str");
		if (debug_str) {
			debug_str_len = debug_str->size;
			debug_str_buf = calloc (1, debug_str_len + 1);
			ret = r_buf_read_at (binfile->buf, debug_str->paddr,
					     debug_str_buf, debug_str_len);
			if (!ret) {
				free (debug_str_buf);
				return false;
			}
		}

		len = section->size;
		if (len > (UT32_MAX >> 1) || len < 1) {
			free (debug_str_buf);
			return false;
		}
		buf = calloc (1, len);
		if (!buf) {
			free (debug_str_buf);
			return false;
		}
		if (!r_buf_read_at (binfile->buf, section->paddr, buf, len)) {
			free (debug_str_buf);
			free (buf);
			return false;
		}
		ret = r_bin_dwarf_parse_info_raw (binfile->sdb_addrinfo, da, buf, len,
				debug_str_buf, debug_str_len, mode);
		R_FREE (debug_str_buf);
		free (buf);
		return ret;
	}
	return false;
}