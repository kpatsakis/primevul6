static ut64 baddr(RBinFile *bf) {
	r_return_val_if_fail (bf && bf->o && bf->o->bin_obj, UT64_MAX);
	struct MACH0_(obj_t) *bin = bf->o->bin_obj;
	return MACH0_(get_baddr)(bin);
}