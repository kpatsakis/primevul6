R_API RBinAddr *r_bin_java_get_entrypoint(RBinJavaObj *bin, int sym) {
	RBinAddr *ret = NULL;
	ret = R_NEW0 (RBinAddr);
	if (!ret) {
		return NULL;
	}
	ret->paddr = UT64_MAX;
	switch (sym) {
	case R_BIN_SYM_ENTRY:
	case R_BIN_SYM_INIT:
		ret->paddr = r_bin_java_find_method_offset (bin, "<init>");
		if (ret->paddr == UT64_MAX) {
			ret->paddr = r_bin_java_find_method_offset (bin, "<cinit>");
		}
		break;
	case R_BIN_SYM_FINI:
		ret->paddr = UT64_MAX;
		break;
	case R_BIN_SYM_MAIN:
		ret->paddr = r_bin_java_find_method_offset (bin, "main");
		break;
	default:
		ret->paddr = -1;
	}
	if (ret->paddr != -1) {
		ret->paddr += bin->loadaddr;
	}
	return ret;
}