R_API int r_bin_dwarf_parse_info_raw(Sdb *s, RBinDwarfDebugAbbrev *da,
		const ut8 *obuf, size_t len,
		const ut8 *debug_str, size_t debug_str_len, int mode) {
	const ut8 *buf = obuf, *buf_end = obuf + len;
	size_t k, offset = 0;
	int curr_unit = 0;
	RBinDwarfDebugInfo di = {0};
	RBinDwarfDebugInfo *inf = &di;
	bool ret = true;

	if (!da || !s || !obuf) {
		return false;
	}

	if (r_bin_dwarf_init_debug_info (inf) < 0) {
		ret = false;
		goto out;
	}
	while (buf < buf_end) {
		if (inf->length >= inf->capacity)
			break;

		if (r_bin_dwarf_init_comp_unit (&inf->comp_units[curr_unit]) < 0) {
			ret = false;
			curr_unit--;
			goto out_debug_info;
		}

		inf->comp_units[curr_unit].offset = buf - obuf;
		inf->comp_units[curr_unit].hdr.pointer_size = 0;
		inf->comp_units[curr_unit].hdr.abbrev_offset = 0;
		inf->comp_units[curr_unit].hdr.length = READ (buf, ut32);
		inf->comp_units[curr_unit].hdr.version = READ (buf, ut16);

		if (inf->comp_units[curr_unit].hdr.version != 2) {
//			eprintf ("DWARF: version %d is not yet supported.\n",
//					inf->comp_units[curr_unit].hdr.version);
			ret = false;
			goto out_debug_info;
		}
		if (inf->comp_units[curr_unit].hdr.length > len) {
			ret = false;
			goto out_debug_info;
		}

		inf->comp_units[curr_unit].hdr.abbrev_offset = READ (buf, ut32);
		inf->comp_units[curr_unit].hdr.pointer_size = READ (buf, ut8);
		inf->length++;

		/* Linear search FIXME */
		if (da->decls->length >= da->capacity) {
			eprintf ("WARNING: malformed dwarf have not enough buckets for decls.\n");
		}
		const int k_max = R_MIN (da->capacity, da->decls->length);
		for (k = 0; k < k_max; k++) {
			if (da->decls[k].offset ==
				inf->comp_units[curr_unit].hdr.abbrev_offset) {
				offset = k;
				break;
			}
		}

		buf = r_bin_dwarf_parse_comp_unit (s, buf, &inf->comp_units[curr_unit],
			da, offset, debug_str, debug_str_len);

		if (!buf) {
			ret = false;
			goto out_debug_info;
		}

		curr_unit++;
	}

	if (mode == R_CORE_BIN_PRINT) {
		r_bin_dwarf_dump_debug_info (NULL, inf);
	}

out_debug_info:
	for (; curr_unit >= 0; curr_unit--) {
		r_bin_dwarf_free_comp_unit (&inf->comp_units[curr_unit]);
	}
	r_bin_dwarf_free_debug_info (inf);
out:
	return ret;
}