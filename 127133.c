static const ut8* r_bin_dwarf_parse_spec_opcode(
		const RBin *a, const ut8 *obuf, size_t len,
		const RBinDwarfLNPHeader *hdr,
		RBinDwarfSMRegisters *regs,
		ut8 opcode, FILE *f, int mode) {
	// XXX - list is not used
	const ut8 *buf = obuf;
	ut8 adj_opcode = 0;
	ut64 advance_adr;
	RBinFile *binfile = a ? a->cur : NULL;

	if (!obuf || !hdr || !regs) {
		return NULL;
	}

	adj_opcode = opcode - hdr->opcode_base;
	if (!hdr->line_range) {
		// line line-range information. move away
		return NULL;
	}
	advance_adr = adj_opcode / hdr->line_range;
	regs->address += advance_adr;
	regs->line += hdr->line_base + (adj_opcode % hdr->line_range);
	if (f) {
		fprintf (f, "Special opcode %d: ", adj_opcode);
		fprintf (f, "advance Address by %"PFMT64d" to %"PFMT64x" and Line by %d to %"PFMT64d"\n",
			advance_adr, regs->address, hdr->line_base +
			(adj_opcode % hdr->line_range), regs->line);
	}
	if (binfile && binfile->sdb_addrinfo && hdr->file_names) {
		int idx = regs->file -1;
		if (idx >= 0 && idx < hdr->file_names_count) {
			add_sdb_addrline (binfile->sdb_addrinfo, regs->address,
					hdr->file_names[idx].name,
					regs->line, f, mode);
		}
	}
	regs->basic_block = DWARF_FALSE;
	regs->prologue_end = DWARF_FALSE;
	regs->epilogue_begin = DWARF_FALSE;
	regs->discriminator = 0;

	return buf;
}