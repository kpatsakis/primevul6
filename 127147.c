static int r_bin_dwarf_init_comp_unit(RBinDwarfCompUnit *cu) {
	if (!cu) {
		return -EINVAL;
	}
	cu->dies = calloc (sizeof (RBinDwarfDIE), COMP_UNIT_CAPACITY);
	if (!cu->dies) {
		return -ENOMEM;
	}
	cu->capacity = COMP_UNIT_CAPACITY;
	cu->length = 0;
	return 0;
}