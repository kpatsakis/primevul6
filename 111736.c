static bool _patch_reloc(struct MACH0_(obj_t) *bin, RIOBind *iob, struct reloc_t * reloc, ut64 symbol_at) {
	ut64 pc = reloc->addr;
	ut64 ins_len = 0;

	switch (bin->hdr.cputype) {
	case CPU_TYPE_X86_64: {
		switch (reloc->type) {
		case X86_64_RELOC_UNSIGNED:
			break;
		case X86_64_RELOC_BRANCH:
			pc -= 1;
			ins_len = 5;
			break;
		default:
			eprintf ("Warning: unsupported reloc type for X86_64 (%d), please file a bug.\n", reloc->type);
			return false;
		}
		break;
	}
	case CPU_TYPE_ARM64:
	case CPU_TYPE_ARM64_32:
		pc = reloc->addr & ~3;
		ins_len = 4;
		break;
	case CPU_TYPE_ARM:
		break;
	default:
		eprintf ("Warning: unsupported architecture for patching relocs, please file a bug. %s\n", MACH0_(get_cputype_from_hdr)(&bin->hdr));
		return false;
	}

	ut64 val = symbol_at;
	if (reloc->pc_relative) {
		val = symbol_at - pc - ins_len;
	}

	ut8 buf[8];
	r_write_ble (buf, val, false, reloc->size * 8);
	iob->write_at (iob->io, reloc->addr, buf, reloc->size);

	return true;
}