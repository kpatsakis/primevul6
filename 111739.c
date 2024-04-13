static RBinAddr *binsym(RBinFile *bf, int sym) {
	ut64 addr;
	RBinAddr *ret = NULL;
	switch (sym) {
	case R_BIN_SYM_MAIN:
		addr = MACH0_(get_main) (bf->o->bin_obj);
		if (addr == UT64_MAX || !(ret = R_NEW0 (RBinAddr))) {
			return NULL;
		}
		//if (bf->o->info && bf->o->info->bits == 16) {
		// align for thumb
		ret->vaddr = ((addr >> 1) << 1);
		//}
		ret->paddr = ret->vaddr;
		break;
	}
	return ret;
}