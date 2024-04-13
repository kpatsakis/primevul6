static void r_bin_dwarf_row_free(void *p) {
	RBinDwarfRow *row = (RBinDwarfRow*)p;
	free (row->file);
	free (row);
}