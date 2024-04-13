static RList *entries(RBinFile *bf) {
	r_return_val_if_fail (bf && bf->o, NULL);

	RBinAddr *ptr = NULL;
	struct addr_t *entry = NULL;

	RList *ret = r_list_newf (free);
	if (!ret) {
		return NULL;
	}

	int bits = MACH0_(get_bits) (bf->o->bin_obj);
	if (!(entry = MACH0_(get_entrypoint) (bf->o->bin_obj))) {
		return ret;
	}
	if ((ptr = R_NEW0 (RBinAddr))) {
		ptr->paddr = entry->offset + bf->o->boffset;
		ptr->vaddr = entry->addr;
		ptr->hpaddr = entry->haddr;
		ptr->bits = bits;
		//realign due to thumb
		if (bits == 16) {
			if (ptr->vaddr & 1) {
				ptr->paddr--;
				ptr->vaddr--;
			}
		}
		r_list_append (ret, ptr);
	}

	process_constructors (bf, ret, bits);
	// constructors
	free (entry);
	return ret;
}