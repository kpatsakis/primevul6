RCoreSymCacheElement *r_coresym_cache_element_new(RBinFile *bf, RBuffer *buf, ut64 off, int bits, char * file_name) {
	RCoreSymCacheElement *result = NULL;
	ut8 *b = NULL;
	RCoreSymCacheElementHdr *hdr = r_coresym_cache_element_header_new (buf, off, bits);
	if (!hdr) {
		return NULL;
	}
	if (hdr->version != 1) {
		eprintf ("Unsupported CoreSymbolication cache version (%d)\n", hdr->version);
		goto beach;
	}
	if (hdr->size == 0 || hdr->size > r_buf_size (buf) - off) {
		eprintf ("Corrupted CoreSymbolication header: size out of bounds (0x%x)\n", hdr->size);
		goto beach;
	}
	result = R_NEW0 (RCoreSymCacheElement);
	if (!result) {
		goto beach;
	}
	result->hdr = hdr;
	b = malloc (hdr->size);
	if (!b) {
		goto beach;
	}
	if (r_buf_read_at (buf, off, b, hdr->size) != hdr->size) {
		goto beach;
	}
	ut8 *end = b + hdr->size;
	if (file_name) {
		result->file_name = file_name;
	} else if (hdr->file_name_off) {
		result->file_name = str_dup_safe (b, b + (size_t)hdr->file_name_off, end);
	}
	if (hdr->version_off) {
		result->binary_version = str_dup_safe (b, b + (size_t)hdr->version_off, end);
	}
	const size_t word_size = bits / 8;
	const ut64 start_of_sections = (ut64)hdr->n_segments * R_CS_EL_SIZE_SEG + R_CS_EL_OFF_SEGS;
	const ut64 sect_size = (bits == 32) ? R_CS_EL_SIZE_SECT_32 : R_CS_EL_SIZE_SECT_64;
	const ut64 start_of_symbols = start_of_sections + (ut64)hdr->n_sections * sect_size;
	const ut64 start_of_lined_symbols = start_of_symbols + (ut64)hdr->n_symbols * R_CS_EL_SIZE_SYM;
	const ut64 start_of_line_info = start_of_lined_symbols + (ut64)hdr->n_lined_symbols * R_CS_EL_SIZE_LSYM;
	const ut64 start_of_unknown_pairs = start_of_line_info + (ut64)hdr->n_line_info * R_CS_EL_SIZE_LINFO;
	const ut64 start_of_strings = start_of_unknown_pairs + (ut64)hdr->n_symbols * 8;

	ut64 page_zero_size = 0;
	size_t page_zero_idx = 0;
	if (UT32_MUL_OVFCHK (hdr->n_segments, sizeof (RCoreSymCacheElementSegment))) {
		goto beach;
	} else if (UT32_MUL_OVFCHK (hdr->n_sections, sizeof (RCoreSymCacheElementSection))) {
		goto beach;
	} else if (UT32_MUL_OVFCHK (hdr->n_symbols, sizeof (RCoreSymCacheElementSymbol))) {
		goto beach;
	} else if (UT32_MUL_OVFCHK (hdr->n_lined_symbols, sizeof (RCoreSymCacheElementLinedSymbol))) {
		goto beach;
	} else if (UT32_MUL_OVFCHK (hdr->n_line_info, sizeof (RCoreSymCacheElementLineInfo))) {
		goto beach;
	}
	if (hdr->n_segments > 0) {
		result->segments = R_NEWS0 (RCoreSymCacheElementSegment, hdr->n_segments);
		if (!result->segments) {
			goto beach;
		}
		size_t i;
		ut8 *cursor = b + R_CS_EL_OFF_SEGS;
		for (i = 0; i < hdr->n_segments && cursor < end; i++) {
			RCoreSymCacheElementSegment *seg = &result->segments[i];
			seg->paddr = seg->vaddr = r_read_le64 (cursor);
			cursor += 8;
			if (cursor >= end) {
				break;
			}
			seg->size = seg->vsize = r_read_le64 (cursor);
			cursor += 8;
			if (cursor >= end) {
				break;
			}
			seg->name = str_dup_safe_fixed (b, cursor, 16, end);
			cursor += 16;
			if (!seg->name) {
				continue;
			}

			if (!strcmp (seg->name, "__PAGEZERO")) {
				page_zero_size = seg->size;
				page_zero_idx = i;
				seg->paddr = seg->vaddr = 0;
				seg->size = 0;
			}
		}
		for (i = 0; i < hdr->n_segments && page_zero_size > 0; i++) {
			if (i == page_zero_idx) {
				continue;
			}
			RCoreSymCacheElementSegment *seg = &result->segments[i];
			if (seg->vaddr < page_zero_size) {
				seg->vaddr += page_zero_size;
			}
		}
	}
	bool relative_to_strings = false;
	ut8* string_origin;
	if (hdr->n_sections > 0) {
		result->sections = R_NEWS0 (RCoreSymCacheElementSection, hdr->n_sections);
		if (!result->sections) {
			goto beach;
		}
		size_t i;
		ut8 *cursor = b + start_of_sections;
		for (i = 0; i < hdr->n_sections && cursor < end; i++) {
			ut8 *sect_start = cursor;
			RCoreSymCacheElementSection *sect = &result->sections[i];
			sect->vaddr = sect->paddr = r_read_ble (cursor, false, bits);
			if (sect->vaddr < page_zero_size) {
				sect->vaddr += page_zero_size;
			}
			cursor += word_size;
			if (cursor >= end) {
				break;
			}
			sect->size = r_read_ble (cursor, false, bits);
			cursor += word_size;
			if (cursor >= end) {
				break;
			}
			ut64 sect_name_off = r_read_ble (cursor, false, bits);
			if (!i && !sect_name_off) {
				relative_to_strings = true;
			}
			cursor += word_size;
			if (bits == 32) {
				cursor += word_size;
			}
			string_origin = relative_to_strings? b + start_of_strings : sect_start;
			sect->name = str_dup_safe (b, string_origin + (size_t)sect_name_off, end);
		}
	}
	if (hdr->n_symbols) {
		result->symbols = R_NEWS0 (RCoreSymCacheElementSymbol, hdr->n_symbols);
		if (!result->symbols) {
			goto beach;
		}
		size_t i;
		ut8 *cursor = b + start_of_symbols;
		for (i = 0; i < hdr->n_symbols && cursor + R_CS_EL_SIZE_SYM <= end; i++) {
			RCoreSymCacheElementSymbol *sym = &result->symbols[i];
			sym->paddr = r_read_le32 (cursor);
			sym->size = r_read_le32 (cursor + 0x4);
			sym->unk1 = r_read_le32 (cursor + 0x8);
			size_t name_off = r_read_le32 (cursor + 0xc);
			size_t mangled_name_off = r_read_le32 (cursor + 0x10);
			sym->unk2 = (st32)r_read_le32 (cursor + 0x14);
			string_origin = relative_to_strings? b + start_of_strings : cursor;
			sym->name = str_dup_safe (b, string_origin + name_off, end);
			if (!sym->name) {
				cursor += R_CS_EL_SIZE_SYM;
				continue;
			}
			string_origin = relative_to_strings? b + start_of_strings : cursor;
			sym->mangled_name = str_dup_safe (b, string_origin + mangled_name_off, end);
			if (!sym->mangled_name) {
				cursor += R_CS_EL_SIZE_SYM;
				continue;
			}
			cursor += R_CS_EL_SIZE_SYM;
		}
	}
	if (hdr->n_lined_symbols) {
		result->lined_symbols = R_NEWS0 (RCoreSymCacheElementLinedSymbol, hdr->n_lined_symbols);
		if (!result->lined_symbols) {
			goto beach;
		}
		size_t i;
		ut8 *cursor = b + start_of_lined_symbols;
		for (i = 0; i < hdr->n_lined_symbols && cursor + R_CS_EL_SIZE_LSYM <= end; i++) {
			RCoreSymCacheElementLinedSymbol *lsym = &result->lined_symbols[i];
			lsym->sym.paddr = r_read_le32 (cursor);
			lsym->sym.size = r_read_le32 (cursor + 0x4);
			lsym->sym.unk1 = r_read_le32 (cursor + 0x8);
			size_t name_off = r_read_le32 (cursor + 0xc);
			size_t mangled_name_off = r_read_le32 (cursor + 0x10);
			lsym->sym.unk2 = (st32)r_read_le32 (cursor + 0x14);
			size_t file_name_off = r_read_le32 (cursor + 0x18);
			lsym->flc.line = r_read_le32 (cursor + 0x1c);
			lsym->flc.col = r_read_le32 (cursor + 0x20);
			string_origin = relative_to_strings? b + start_of_strings : cursor;
			lsym->sym.name = str_dup_safe (b, string_origin + name_off, end);
			if (!lsym->sym.name) {
				cursor += R_CS_EL_SIZE_LSYM;
				continue;
			}
			string_origin = relative_to_strings? b + start_of_strings : cursor;
			lsym->sym.mangled_name = str_dup_safe (b, string_origin + mangled_name_off, end);
			if (!lsym->sym.mangled_name) {
				cursor += R_CS_EL_SIZE_LSYM;
				continue;
			}
			string_origin = relative_to_strings? b + start_of_strings : cursor;
			lsym->flc.file = str_dup_safe (b, string_origin + file_name_off, end);
			if (!lsym->flc.file) {
				cursor += R_CS_EL_SIZE_LSYM;
				continue;
			}
			cursor += R_CS_EL_SIZE_LSYM;
			meta_add_fileline (bf, r_coresym_cache_element_pa2va (result, lsym->sym.paddr), lsym->sym.size, &lsym->flc);
		}
	}
	if (hdr->n_line_info) {
		result->line_info = R_NEWS0 (RCoreSymCacheElementLineInfo, hdr->n_line_info);
		if (!result->line_info) {
			goto beach;
		}
		size_t i;
		ut8 *cursor = b + start_of_line_info;
		for (i = 0; i < hdr->n_line_info && cursor + R_CS_EL_SIZE_LINFO <= end; i++) {
			RCoreSymCacheElementLineInfo *info = &result->line_info[i];
			info->paddr = r_read_le32 (cursor);
			info->size = r_read_le32 (cursor + 4);
			size_t file_name_off = r_read_le32 (cursor + 8);
			info->flc.line = r_read_le32 (cursor + 0xc);
			info->flc.col = r_read_le32 (cursor + 0x10);
			string_origin = relative_to_strings? b + start_of_strings : cursor;
			info->flc.file = str_dup_safe (b, string_origin + file_name_off, end);
			if (!info->flc.file) {
				break;
			}
			cursor += R_CS_EL_SIZE_LINFO;
			meta_add_fileline (bf, r_coresym_cache_element_pa2va (result, info->paddr), info->size, &info->flc);
		}
	}

	/*
	 * TODO:
	 * Figure out the meaning of the 2 arrays of hdr->n_symbols
	 * 32-bit integers located at the end of line info.
	 * Those are the last info before the strings at the end.
	 */

beach:
	free (b);
	return result;
}