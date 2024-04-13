static void destroy(RBinFile *bf) {
	MACH0_(mach0_free) (bf->o->bin_obj);
}