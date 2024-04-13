static void r_bin_dwarf_free_comp_unit(RBinDwarfCompUnit *cu) {
	size_t i;
	if (!cu) {
		return;
	}
	for (i = 0; i < cu->length; i++) {
		if (cu->dies) {
			r_bin_dwarf_free_die (&cu->dies[i]);
		}
	}
	R_FREE (cu->dies);
}