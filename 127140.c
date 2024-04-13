static const ut8* r_bin_dwarf_parse_opcodes(const RBin *a, const ut8 *obuf,
		size_t len, const RBinDwarfLNPHeader *hdr,
		RBinDwarfSMRegisters *regs, FILE *f, int mode) {
	const ut8 *buf, *buf_end;
	ut8 opcode, ext_opcode;

	if (!a || !obuf || len < 8) {
		return NULL;
	}
	buf = obuf;
	buf_end = obuf + len;

	while (buf && buf + 1 < buf_end) {
		opcode = *buf++;
		len--;
		if (!opcode) {
			ext_opcode = *buf;
			buf = r_bin_dwarf_parse_ext_opcode (a, buf, len, hdr, regs, f, mode);
			if (ext_opcode == DW_LNE_end_sequence) {
				break;
			}
		} else if (opcode >= hdr->opcode_base) {
			buf = r_bin_dwarf_parse_spec_opcode (a, buf, len, hdr, regs, opcode, f, mode);
		} else {
			buf = r_bin_dwarf_parse_std_opcode (a, buf, len, hdr, regs, opcode, f, mode);
		}
		len = (int)(buf_end - buf);
	}
	return buf;
}