static ut64 size(RBinFile *bf) {
	ut64 off = 0;
	ut64 len = 0;
	if (!bf->o->sections) {
		RListIter *iter;
		RBinSection *section;
		bf->o->sections = sections (bf);
		r_list_foreach (bf->o->sections, iter, section) {
			if (section->paddr > off) {
				off = section->paddr;
				len = section->size;
			}
		}
	}
	return off + len;
}