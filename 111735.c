static RList *sections(RBinFile *bf) {
	return MACH0_(get_segments) (bf);
}