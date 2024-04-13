static int r_bin_dwarf_init_debug_abbrev(RBinDwarfDebugAbbrev *da) {
	if (!da) {
		return -EINVAL;
	}
	da->decls = calloc (sizeof (RBinDwarfAbbrevDecl), DEBUG_ABBREV_CAP);
	if (!da->decls) {
		return -ENOMEM;
	}
	da->capacity = DEBUG_ABBREV_CAP;
	da->length = 0;

	return 0;
}