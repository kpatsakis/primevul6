static inline vm_fault_t hugetlb_handle_userfault(struct vm_area_struct *vma,
						  struct address_space *mapping,
						  pgoff_t idx,
						  unsigned int flags,
						  unsigned long haddr,
						  unsigned long reason)
{
	vm_fault_t ret;
	u32 hash;
	struct vm_fault vmf = {
		.vma = vma,
		.address = haddr,
		.flags = flags,

		/*
		 * Hard to debug if it ends up being
		 * used by a callee that assumes
		 * something about the other
		 * uninitialized fields... same as in
		 * memory.c
		 */
	};

	/*
	 * hugetlb_fault_mutex and i_mmap_rwsem must be
	 * dropped before handling userfault.  Reacquire
	 * after handling fault to make calling code simpler.
	 */
	hash = hugetlb_fault_mutex_hash(mapping, idx);
	mutex_unlock(&hugetlb_fault_mutex_table[hash]);
	i_mmap_unlock_read(mapping);
	ret = handle_userfault(&vmf, reason);
	i_mmap_lock_read(mapping);
	mutex_lock(&hugetlb_fault_mutex_table[hash]);

	return ret;
}