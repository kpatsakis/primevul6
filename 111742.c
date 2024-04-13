static RList* patch_relocs(RBin *b) {
	RList *ret = NULL;
	RIO *io = NULL;
	RBinObject *obj = NULL;
	struct MACH0_(obj_t) *bin = NULL;
	RIOMap *g = NULL;
	HtUU *relocs_by_sym = NULL;
	RIODesc *gotr2desc = NULL;

	r_return_val_if_fail (b, NULL);

	io = b->iob.io;
	if (!io || !io->desc) {
		return NULL;
	}
	obj = r_bin_cur_object (b);
	if (!obj) {
		return NULL;
	}
	bin = obj->bin_obj;

	RSkipList * all_relocs = MACH0_(get_relocs)(bin);
	if (!all_relocs) {
		return NULL;
	}
	RList * ext_relocs = r_list_new ();
	if (!ext_relocs) {
		goto beach;
	}
	RSkipListNode *it;
	struct reloc_t * reloc;
	r_skiplist_foreach (all_relocs, it, reloc) {
		if (!reloc->external) {
			continue;
		}
		r_list_append (ext_relocs, reloc);
	}
	ut64 num_ext_relocs = r_list_length (ext_relocs);
	if (!num_ext_relocs) {
		goto beach;
	}

	if (!io->cached) {
		eprintf ("Warning: run r2 with -e bin.cache=true to fix relocations in disassembly\n");
		goto beach;
	}

	int cdsz = obj->info ? obj->info->bits / 8 : 8;

	ut64 offset = 0;
	void **vit;
	r_pvector_foreach (&io->maps, vit) {
		RIOMap *map = *vit;
		if (r_io_map_begin (map) > offset) {
			offset = r_io_map_begin (map);
			g = map;
		}
	}
	if (!g) {
		goto beach;
	}
	ut64 n_vaddr = g->itv.addr + g->itv.size;
	ut64 size = num_ext_relocs * cdsz;
	char *muri = r_str_newf ("malloc://%" PFMT64u, size);
	gotr2desc = b->iob.open_at (io, muri, R_PERM_R, 0664, n_vaddr);
	free (muri);
	if (!gotr2desc) {
		goto beach;
	}

	RIOMap *gotr2map = b->iob.map_get_at (io, n_vaddr);
	if (!gotr2map) {
		goto beach;
	}
	gotr2map->name = strdup (".got.r2");

	if (!(ret = r_list_newf ((RListFree)free))) {
		goto beach;
	}
	if (!(relocs_by_sym = ht_uu_new0 ())) {
		goto beach;
	}
	ut64 vaddr = n_vaddr;
	RListIter *liter;
	r_list_foreach (ext_relocs, liter, reloc) {
		ut64 sym_addr = 0;
		sym_addr = ht_uu_find (relocs_by_sym, reloc->ord, NULL);
		if (!sym_addr) {
			sym_addr = vaddr;
			ht_uu_insert (relocs_by_sym, reloc->ord, vaddr);
			vaddr += cdsz;
		}
		if (!_patch_reloc (bin, &b->iob, reloc, sym_addr)) {
			continue;
		}
		RBinReloc *ptr = NULL;
		if (!(ptr = R_NEW0 (RBinReloc))) {
			goto beach;
		}
		ptr->type = reloc->type;
		ptr->additive = 0;
		RBinImport *imp;
		if (!(imp = import_from_name (b, (char*) reloc->name, bin->imports_by_name))) {
			R_FREE (ptr);
			goto beach;
		}
		ptr->vaddr = sym_addr;
		ptr->import = imp;
		r_list_append (ret, ptr);
	}
	if (r_list_empty (ret)) {
		goto beach;
	}
	ht_uu_free (relocs_by_sym);
	r_list_free (ext_relocs);
	r_skiplist_free (all_relocs);
	return ret;

beach:
	r_list_free (ext_relocs);
	r_skiplist_free (all_relocs);
	r_io_desc_free (gotr2desc);
	r_list_free (ret);
	ht_uu_free (relocs_by_sym);
	return NULL;
}