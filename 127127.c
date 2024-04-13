R_API int r_bin_dwarf_parse_line_raw2(const RBin *a, const ut8 *obuf,
				       size_t len, int mode) {
	RBinDwarfLNPHeader hdr = {{0}};
	const ut8 *buf = NULL, *buf_tmp = NULL, *buf_end = NULL;
	RBinDwarfSMRegisters regs;
	int tmplen;
	FILE *f = NULL;
	RBinFile *binfile = a ? a->cur : NULL;

	if (!binfile || !obuf) {
		return false;
	}
	if (mode == R_CORE_BIN_PRINT) {
		f = stdout;
	}
	buf = obuf;
	buf_end = obuf + len;
	while (buf + 1 < buf_end) {
		buf_tmp = buf;
		buf = r_bin_dwarf_parse_lnp_header (a->cur, buf, buf_end, &hdr, f, mode);
		if (!buf) {
			return false;
		}
		r_bin_dwarf_set_regs_default (&hdr, &regs);
		tmplen = (int)(buf_end - buf);
		tmplen = R_MIN (tmplen, 4 + hdr.unit_length.part1);
		if (tmplen < 1) {
			break;
		}
		if (!r_bin_dwarf_parse_opcodes (a, buf, tmplen, &hdr, &regs, f, mode)) {
			break;
		}
		buf = buf_tmp + tmplen;
		len = (int)(buf_end - buf);
	}
	return true;
}