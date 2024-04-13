static RBinDwarfDebugAbbrev *r_bin_dwarf_parse_abbrev_raw(const ut8 *obuf, size_t len, int mode) {
	const ut8 *buf = obuf, *buf_end = obuf + len;
	ut64 tmp, spec1, spec2, offset;
	ut8 has_children;
	RBinDwarfAbbrevDecl *tmpdecl;

	// XXX - Set a suitable value here.
	if (!obuf || len < 3) {
		return NULL;
	}
	RBinDwarfDebugAbbrev *da = R_NEW0 (RBinDwarfDebugAbbrev);

	r_bin_dwarf_init_debug_abbrev (da);

	while (buf && buf+1 < buf_end) {
		offset = buf - obuf;
		buf = r_uleb128 (buf, (size_t)(buf_end-buf), &tmp);
		if (!buf || !tmp) {
			continue;
		}
		if (da->length == da->capacity) {
			r_bin_dwarf_expand_debug_abbrev(da);
		}
		tmpdecl = &da->decls[da->length];
		r_bin_dwarf_init_abbrev_decl (tmpdecl);

		tmpdecl->code = tmp;
		buf = r_uleb128 (buf, (size_t)(buf_end-buf), &tmp);
		tmpdecl->tag = tmp;

		tmpdecl->offset = offset;
		if (buf >= buf_end) {
			break;
		}
		has_children = READ (buf, ut8);
		tmpdecl->has_children = has_children;
		do {
			if (tmpdecl->length == tmpdecl->capacity) {
				r_bin_dwarf_expand_abbrev_decl (tmpdecl);
			}
			buf = r_uleb128 (buf, (size_t)(buf_end - buf), &spec1);
			buf = r_uleb128 (buf, (size_t)(buf_end - buf), &spec2);
			tmpdecl->specs[tmpdecl->length].attr_name = spec1;
			tmpdecl->specs[tmpdecl->length].attr_form = spec2;
			tmpdecl->length++;
		} while (spec1 && spec2);

		da->length++;
	}

	if (mode == R_CORE_BIN_PRINT) {
		dump_r_bin_dwarf_debug_abbrev (stdout, da);
	}
	return da;
}