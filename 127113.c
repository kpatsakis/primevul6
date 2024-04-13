static int r_bin_dwarf_expand_die(RBinDwarfDIE* die) {
	RBinDwarfAttrValue *tmp = NULL;
	if (!die || die->capacity == 0) {
		return -EINVAL;
	}
	if (die->capacity != die->length) {
		return -EINVAL;
	}
	tmp = (RBinDwarfAttrValue*)realloc (die->attr_values,
			die->capacity * 2 * sizeof (RBinDwarfAttrValue));
	if (!tmp) {
		return -ENOMEM;
	}
	memset ((ut8*)tmp + die->capacity, 0, die->capacity);
	die->attr_values = tmp;
	die->capacity *= 2;
	return 0;
}