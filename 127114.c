static void r_bin_dwarf_free_die(RBinDwarfDIE *die) {
	size_t i;
	if (!die) {
		return;
	}
	for (i = 0; i < die->length; i++) {
		r_bin_dwarf_free_attr_value (&die->attr_values[i]);
	}
	R_FREE (die->attr_values);
}