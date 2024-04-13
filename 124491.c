ut64 r_bin_mdmp_get_paddr(struct r_bin_mdmp_obj *obj, ut64 vaddr) {
	/* FIXME: Will only resolve exact matches, probably no need to fix as
	** this function will become redundant on the optimisation stage */
	struct minidump_memory_descriptor64 *memory;
	ut64 index, paddr = 0;
	RListIter *it;

	/* Loop through the memories sections looking for a match */
	index = obj->streams.memories64.base_rva;
	r_list_foreach (obj->streams.memories64.memories, it, memory) {
		if (vaddr == memory->start_of_memory_range) {
			paddr = index;
			break;
		}
		index += memory->data_size;
	}
	return paddr;
}