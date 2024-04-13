static int r_bin_dwarf_init_debug_info(RBinDwarfDebugInfo *inf) {
	if (!inf) {
		return -1;
	}
	inf->comp_units = calloc (sizeof (RBinDwarfCompUnit), DEBUG_INFO_CAPACITY);

	// XXX - should we be using error codes?
	if (!inf->comp_units) {
		return -ENOMEM;
	}

	inf->capacity = DEBUG_INFO_CAPACITY;
	inf->length = 0;

	return true;
}