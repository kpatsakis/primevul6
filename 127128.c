static RBinDwarfRow *r_bin_dwarf_row_new (ut64 addr, const char *file, int line, int col) {
	RBinDwarfRow *row = R_NEW0 (RBinDwarfRow);
	if (!row) {
		return NULL;
	}
	row->file = strdup (file);
	row->address = addr;
	row->line = line;
	row->column = 0;
	return row;
}