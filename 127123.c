static const ut8* r_bin_dwarf_parse_ext_opcode(const RBin *a, const ut8 *obuf,
		size_t len, const RBinDwarfLNPHeader *hdr,
		RBinDwarfSMRegisters *regs, FILE *f, int mode) {
	// XXX - list is an unused parameter.
	const ut8 *buf;
	const ut8 *buf_end;
	ut8 opcode;
	ut64 addr;
	buf = obuf;
	st64 op_len;
	RBinFile *binfile = a ? a->cur : NULL;
	RBinObject *o = binfile ? binfile->o : NULL;
	ut32 addr_size = o && o->info && o->info->bits ? o->info->bits / 8 : 4;
	const char *filename;

	if (!binfile || !obuf || !hdr || !regs) return NULL;

	buf = r_leb128 (buf, &op_len);
	buf_end = buf+len;
	opcode = *buf++;

	if (f) {
		fprintf (f, "Extended opcode %d: ", opcode);
	}

	switch (opcode) {
	case DW_LNE_end_sequence:
		regs->end_sequence = DWARF_TRUE;

		if (binfile && binfile->sdb_addrinfo && hdr->file_names) {
			int fnidx = regs->file - 1;
			if (fnidx >= 0 && fnidx < hdr->file_names_count) {
				add_sdb_addrline(binfile->sdb_addrinfo, regs->address,
						hdr->file_names[fnidx].name, regs->line, f, mode);
			}
		}

		if (f) {
			fprintf (f, "End of Sequence\n");
		}
		break;
	case DW_LNE_set_address:
		if (addr_size == 8) {
			addr = READ (buf, ut64);
		} else {
			addr = READ (buf, ut32);
		}

		regs->address = addr;

		if (f) {
			fprintf (f, "set Address to 0x%"PFMT64x"\n", addr);
		}
		break;
	case DW_LNE_define_file:
		filename = (const char*)buf;

		if (f) {
			fprintf (f, "define_file\n");
			fprintf (f, "filename %s\n", filename);
		}

		buf += (strlen (filename) + 1);
		ut64 dir_idx;
		if (buf+1 < buf_end)
			buf = r_uleb128 (buf, ST32_MAX, &dir_idx);
		break;
	case DW_LNE_set_discriminator:
		buf = r_uleb128 (buf, ST32_MAX, &addr);
		if (f) {
			fprintf (f, "set Discriminator to %"PFMT64d"\n", addr);
		}
		regs->discriminator = addr;
		break;
	default:
		if (f) {
			fprintf (f, "Unexpeced opcode %d\n", opcode);
		}
		break;
	}

	return buf;
}