static RList *relocs(RBinFile *bf) {
	RList *ret = NULL;
	struct MACH0_(obj_t) *bin = NULL;
	RBinObject *obj = bf ? bf->o : NULL;
	if (bf && bf->o) {
		bin = bf->o->bin_obj;
	}
	if (!obj || !obj->bin_obj || !(ret = r_list_newf (free))) {
		return NULL;
	}
	ret->free = free;

	RSkipList *relocs;
	if (!(relocs = MACH0_(get_relocs) (bf->o->bin_obj))) {
		return ret;
	}

	RSkipListNode *it;
	struct reloc_t *reloc;
	r_skiplist_foreach (relocs, it, reloc) {
		if (reloc->external) {
			continue;
		}
		RBinReloc *ptr = NULL;
		if (!(ptr = R_NEW0 (RBinReloc))) {
			break;
		}
		ptr->type = reloc->type;
		ptr->additive = 0;
		if (reloc->name[0]) {
			RBinImport *imp;
			if (!(imp = import_from_name (bf->rbin, (char*) reloc->name, bin->imports_by_name))) {
				free (ptr);
				break;
			}
			ptr->import = imp;
		} else if (reloc->ord >= 0 && bin->imports_by_ord && reloc->ord < bin->imports_by_ord_size) {
			ptr->import = bin->imports_by_ord[reloc->ord];
		} else {
			ptr->import = NULL;
		}
		ptr->addend = reloc->addend;
		ptr->vaddr = reloc->addr;
		ptr->paddr = reloc->offset;
		r_list_append (ret, ptr);
	}

	r_skiplist_free (relocs);

	return ret;
}