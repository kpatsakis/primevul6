static int r_bin_dwarf_expand_abbrev_decl(RBinDwarfAbbrevDecl *ad) {
	RBinDwarfAttrSpec *tmp;

	if (!ad || !ad->capacity || ad->capacity != ad->length) {
		return -EINVAL;
	}

	tmp = (RBinDwarfAttrSpec*)realloc (ad->specs,
			ad->capacity * 2 * sizeof (RBinDwarfAttrSpec));

	if (!tmp) {
		return -ENOMEM;
	}

	memset ((ut8*)tmp + ad->capacity, 0, ad->capacity);
	ad->specs = tmp;
	ad->capacity *= 2;

	return 0;
}