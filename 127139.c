R_API RBinDwarfDebugAbbrev *r_bin_dwarf_parse_abbrev(RBin *a, int mode) {
	ut8 *buf;
	size_t len;
	RBinSection *section = getsection (a, "debug_abbrev");
	RBinDwarfDebugAbbrev *da = NULL;
	RBinFile *binfile = a ? a->cur: NULL;
	if (!section || !binfile) return NULL;
	if (section->size > binfile->size) {
		return NULL;
	}
	len = section->size;
	buf = calloc (1,len);
	r_buf_read_at (binfile->buf, section->paddr, buf, len);
	da = r_bin_dwarf_parse_abbrev_raw (buf, len, mode);
	free (buf);
	return da;
}