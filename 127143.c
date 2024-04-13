static int r_bin_dwarf_init_die(RBinDwarfDIE *die) {
	if (!die) {
		return -EINVAL;
	}
	die->attr_values = calloc (sizeof (RBinDwarfAttrValue), 8);
	if (!die->attr_values) {
		return -ENOMEM;
	}
	die->capacity = 8;
	die->length = 0;
	return 0;
}