static int r_bin_dwarf_expand_cu(RBinDwarfCompUnit *cu) {
	RBinDwarfDIE *tmp;

	if (!cu || cu->capacity == 0 || cu->capacity != cu->length) {
		return -EINVAL;
	}

	tmp = (RBinDwarfDIE*)realloc(cu->dies,
			cu->capacity * 2 * sizeof(RBinDwarfDIE));
	if (!tmp) {
		return -ENOMEM;
	}

	memset ((ut8*)tmp + cu->capacity, 0, cu->capacity);
	cu->dies = tmp;
	cu->capacity *= 2;

	return 0;
}