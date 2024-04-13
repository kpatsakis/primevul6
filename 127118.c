static int r_bin_dwarf_expand_debug_abbrev(RBinDwarfDebugAbbrev *da) {
	RBinDwarfAbbrevDecl *tmp;

	if (!da || da->capacity == 0 || da->capacity != da->length) {
		return -EINVAL;
	}

	tmp = (RBinDwarfAbbrevDecl*)realloc (da->decls,
			da->capacity * 2 * sizeof (RBinDwarfAbbrevDecl));

	if (!tmp) {
		return -ENOMEM;
	}
	memset ((ut8*)tmp + da->capacity, 0, da->capacity);

	da->decls = tmp;
	da->capacity *= 2;

	return 0;
}