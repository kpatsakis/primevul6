static int r_bin_dwarf_init_abbrev_decl(RBinDwarfAbbrevDecl *ad) {
	if (!ad) {
		return -EINVAL;
	}
	ad->specs = calloc (sizeof( RBinDwarfAttrSpec), ABBREV_DECL_CAP);

	if (!ad->specs) {
		return -ENOMEM;
	}

	ad->capacity = ABBREV_DECL_CAP;
	ad->length = 0;

	return 0;
}