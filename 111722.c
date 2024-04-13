static RBinInfo *info(RBinFile *bf) {
	struct MACH0_(obj_t) *bin = NULL;
	char *str;

	r_return_val_if_fail (bf && bf->o, NULL);
	RBinInfo *ret = R_NEW0 (RBinInfo);
	if (!ret) {
		return NULL;
	}

	bin = bf->o->bin_obj;
	if (bf->file) {
		ret->file = strdup (bf->file);
	}
	if ((str = MACH0_(get_class) (bf->o->bin_obj))) {
		ret->bclass = str;
	}
	if (bin) {
		ret->has_canary = bin->has_canary;
		ret->has_retguard = -1;
		ret->has_sanitizers = bin->has_sanitizers;
		ret->dbg_info = bin->dbg_info;
		ret->lang = bin->lang;
	}
	ret->intrp = r_str_dup (NULL, MACH0_(get_intrp)(bf->o->bin_obj));
	ret->compiler = r_str_dup (NULL, "");
	ret->rclass = strdup ("mach0");
	ret->os = strdup (MACH0_(get_os)(bf->o->bin_obj));
	ret->subsystem = strdup ("darwin");
	ret->arch = strdup (MACH0_(get_cputype) (bf->o->bin_obj));
	ret->machine = MACH0_(get_cpusubtype) (bf->o->bin_obj);
	ret->has_lit = true;
	ret->type = MACH0_(get_filetype) (bf->o->bin_obj);
	ret->big_endian = MACH0_(is_big_endian) (bf->o->bin_obj);
	ret->bits = 32;
	if (bf && bf->o && bf->o->bin_obj) {
		ret->has_crypto = ((struct MACH0_(obj_t)*)
			bf->o->bin_obj)->has_crypto;
		ret->bits = MACH0_(get_bits) (bf->o->bin_obj);
	}
	ret->has_va = true;
	ret->has_pi = MACH0_(is_pie) (bf->o->bin_obj);
	ret->has_nx = MACH0_(has_nx) (bf->o->bin_obj);
	return ret;
}