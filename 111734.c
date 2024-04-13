static RList *imports(RBinFile *bf) {
	RBinObject *obj = bf ? bf->o : NULL;
	struct MACH0_(obj_t) *bin = bf ? bf->o->bin_obj : NULL;
	const char *name;
	RBinImport *ptr = NULL;
	int i;

	if (!obj || !bin || !obj->bin_obj) {
		return NULL;
	}
	RList *ret = r_list_newf((RListFree)r_bin_import_free);
	struct import_t *imports = MACH0_(get_imports)(bf->o->bin_obj);
	if (!ret || !imports) {
		r_list_free (ret);
		free (imports);
		return NULL;
	}
	bin->has_canary = false;
	bin->has_retguard = -1;
	bin->has_sanitizers = false;
	bin->has_blocks_ext = false;
	for (i = 0; !imports[i].last; i++) {
		if (!(ptr = import_from_name (bf->rbin, imports[i].name, NULL))) {
			break;
		}
		name = ptr->name;
		ptr->ordinal = imports[i].ord;
		if (bin->imports_by_ord && ptr->ordinal < bin->imports_by_ord_size) {
			bin->imports_by_ord[ptr->ordinal] = ptr;
		}
		if (!strcmp (name, "__stack_chk_fail") ) {
			bin->has_canary = true;
		}
		if (!strcmp (name, "__asan_init") ||
                   !strcmp (name, "__tsan_init")) {
			bin->has_sanitizers = true;
		}
		if (!strcmp (name, "_NSConcreteGlobalBlock")) {
			bin->has_blocks_ext = true;
		}
		r_list_append (ret, ptr);
	}
	free (imports);
	return ret;
}