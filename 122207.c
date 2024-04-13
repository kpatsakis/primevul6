R_API RList *r_bin_java_get_entrypoints(RBinJavaObj *bin) {
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaField *fm_type;
	RList *ret = r_list_newf (free);
	if (!ret) {
		return NULL;
	}
	r_list_foreach_safe (bin->methods_list, iter, iter_tmp, fm_type) {
		if (!strcmp (fm_type->name, "main")
		|| !strcmp (fm_type->name, "<init>")
		|| !strcmp (fm_type->name, "<clinit>")
		|| strstr (fm_type->flags_str, "static")) {
			RBinAddr *addr = R_NEW0 (RBinAddr);
			if (addr) {
				addr->vaddr = addr->paddr = \
					r_bin_java_get_method_code_offset (fm_type) + bin->loadaddr;
				addr->haddr = fm_type->file_offset;
				r_list_append (ret, addr);
			}
		}
	}
	return ret;
}