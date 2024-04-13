R_API RList *r_bin_dwarf_parse_aranges(RBin *a, int mode) {
	ut8 *buf;
	int ret;
	size_t len;
	RBinSection *section = getsection (a, "debug_aranges");
	RBinFile *binfile = a ? a->cur: NULL;

	if (binfile && section) {
		len = section->size;
		if (len < 1 || len > ST32_MAX) {
			return NULL;
		}
		buf = calloc (1, len);
		ret = r_buf_read_at (binfile->buf, section->paddr, buf, len);
		if (!ret) {
			free (buf);
			return NULL;
		}
		if (mode == R_CORE_BIN_PRINT) {
			r_bin_dwarf_parse_aranges_raw (buf, len, stdout);
		} else {
			r_bin_dwarf_parse_aranges_raw (buf, len, DBGFD);
		}
		free (buf);
	}
	return NULL;
}