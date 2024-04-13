R_API void r_bin_dwarf_free_debug_abbrev(RBinDwarfDebugAbbrev *da) {
	size_t i;
	if (!da) return;
	for (i = 0; i < da->length; i++) {
		R_FREE (da->decls[i].specs);
	}
	R_FREE (da->decls);
}