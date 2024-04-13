static RList *symbols(RBinFile *bf) {
	RBinObject *obj = bf? bf->o: NULL;
	return (RList *)MACH0_(get_symbols_list) (obj->bin_obj);
}