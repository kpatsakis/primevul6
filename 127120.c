static void r_bin_dwarf_free_debug_info (RBinDwarfDebugInfo *inf) {
	size_t i;
	if (!inf) {
		return;
	}
	for (i = 0; i < inf->length; i++) {
		r_bin_dwarf_free_comp_unit (&inf->comp_units[i]);
	}
	R_FREE (inf->comp_units);
}