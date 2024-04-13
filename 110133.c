void exit_mmap(struct mm_struct *mm)
{
	struct mmu_gather tlb;
	struct vm_area_struct *vma;
	unsigned long nr_accounted = 0;

	/* mm's last user has gone, and its about to be pulled down */
	mmu_notifier_release(mm);

	if (mm->locked_vm) {
		vma = mm->mmap;
		while (vma) {
			if (vma->vm_flags & VM_LOCKED)
				munlock_vma_pages_all(vma);
			vma = vma->vm_next;
		}
	}

	arch_exit_mmap(mm);

	vma = mm->mmap;
	if (!vma)	/* Can happen if dup_mmap() received an OOM */
		return;

	lru_add_drain();
	flush_cache_mm(mm);
	tlb_gather_mmu(&tlb, mm, 0, -1);
	/* update_hiwater_rss(mm) here? but nobody should be looking */
	/* Use -1 here to ensure all VMAs in the mm are unmapped */
	unmap_vmas(&tlb, vma, 0, -1);

	free_pgtables(&tlb, vma, FIRST_USER_ADDRESS, USER_PGTABLES_CEILING);
	tlb_finish_mmu(&tlb, 0, -1);

	/*
	 * Walk the list again, actually closing and freeing it,
	 * with preemption enabled, without holding any MM locks.
	 */
	while (vma) {
		if (vma->vm_flags & VM_ACCOUNT)
			nr_accounted += vma_pages(vma);
		vma = remove_vma(vma);
	}
	vm_unacct_memory(nr_accounted);
}