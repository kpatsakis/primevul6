static char *entitlements(RBinFile *bf, bool json) {
	r_return_val_if_fail (bf && bf->o && bf->o->bin_obj, NULL);
	struct MACH0_(obj_t) *bin = bf->o->bin_obj;
	if (json) {
		const char *s = r_str_get ((const char *)bin->signature);
		PJ *pj = pj_new ();
		pj_s (pj, s);
		return pj_drain (pj);
	}
	return r_str_dup (NULL, (const char*)bin->signature);
}