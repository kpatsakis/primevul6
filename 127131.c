static const ut8* r_bin_dwarf_parse_std_opcode(
		const RBin *a, const ut8 *obuf, size_t len,
		const RBinDwarfLNPHeader *hdr, RBinDwarfSMRegisters *regs,
		ut8 opcode, FILE *f, int mode) {
	const ut8* buf = obuf;
	const ut8* buf_end = obuf + len;
	ut64 addr = 0LL;
	st64 sbuf;
	ut8 adj_opcode;
	ut64 op_advance;
	ut16 operand;
	RBinFile *binfile = a ? a->cur : NULL;

	if (!binfile || !hdr || !regs || !obuf) {
		return NULL;
	}
	switch (opcode) {
	case DW_LNS_copy:
		if (f) {
			fprintf (f, "Copy\n");
		}
		if (binfile && binfile->sdb_addrinfo && hdr->file_names) {
			int fnidx = regs->file - 1;
			if (fnidx >= 0 && fnidx < hdr->file_names_count) {
				add_sdb_addrline (binfile->sdb_addrinfo,
					regs->address,
					hdr->file_names[fnidx].name,
					regs->line, f, mode);
			}
		}
		regs->basic_block = DWARF_FALSE;
		break;
	case DW_LNS_advance_pc:
		buf = r_uleb128 (buf, ST32_MAX, &addr);
		regs->address += addr * hdr->min_inst_len;
		if (f) {
			fprintf (f, "Advance PC by %"PFMT64d" to 0x%"PFMT64x"\n",
				addr * hdr->min_inst_len, regs->address);
		}
		break;
	case DW_LNS_advance_line:
		buf = r_leb128(buf, &sbuf);
		regs->line += sbuf;
		if (f) {
			fprintf (f, "Advance line by %"PFMT64d", to %"PFMT64d"\n", sbuf, regs->line);
		}
		break;
	case DW_LNS_set_file:
		buf = r_uleb128 (buf, ST32_MAX, &addr);
		if (f) {
			fprintf (f, "Set file to %"PFMT64d"\n", addr);
		}
		regs->file = addr;
		break;
	case DW_LNS_set_column:
		buf = r_uleb128 (buf, ST32_MAX, &addr);
		if (f) {
			fprintf (f, "Set column to %"PFMT64d"\n", addr);
		}
		regs->column = addr;
		break;
	case DW_LNS_negate_stmt:
		regs->is_stmt = regs->is_stmt ? DWARF_FALSE : DWARF_TRUE;
		if (f) {
			fprintf (f, "Set is_stmt to %d\n", regs->is_stmt);
		}
		break;
	case DW_LNS_set_basic_block:
		if (f) {
			fprintf (f, "set_basic_block\n");
		}
		regs->basic_block = DWARF_TRUE;
		break;
	case DW_LNS_const_add_pc:
		adj_opcode = 255 - hdr->opcode_base;
		if (hdr->line_range > 0) {
			op_advance = adj_opcode / hdr->line_range;
		} else {
			op_advance = 0;
		}
		regs->address += op_advance;
		if (f) {
			fprintf (f, "Advance PC by constant %"PFMT64d" to 0x%"PFMT64x"\n",
				op_advance, regs->address);
		}
		break;
	case DW_LNS_fixed_advance_pc:
		operand = READ (buf, ut16);
		regs->address += operand;
		if (f) {
			fprintf (f,"Fixed advance pc to %"PFMT64d"\n", regs->address);
		}
		break;
	case DW_LNS_set_prologue_end:
		regs->prologue_end = ~0;
		if (f) {
			fprintf (f, "set_prologue_end\n");
		}
		break;
	case DW_LNS_set_epilogue_begin:
		regs->epilogue_begin = ~0;
		if (f) {
			fprintf (f, "set_epilogue_begin\n");
		}
		break;
	case DW_LNS_set_isa:
		buf = r_uleb128 (buf, ST32_MAX, &addr);
		regs->isa = addr;
		if (f) {
			fprintf (f, "set_isa\n");
		}
		break;
	default:
		if (f) {
			fprintf (f, "Unexpected opcode\n");
		}
		break;
	}
	return buf;
}