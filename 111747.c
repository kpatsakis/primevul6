static Sdb *get_sdb (RBinFile *bf) {
	RBinObject *o = bf->o;
	if (!o) {
		return NULL;
	}
	struct MACH0_(obj_t) *bin = (struct MACH0_(obj_t) *) o->bin_obj;
	return bin? bin->kv: NULL;
}