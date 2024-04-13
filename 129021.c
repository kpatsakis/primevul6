void exit_mmap(struct mm_struct *mm)
{
	struct mmu_gather *tlb;
	struct vm_area_struct *vma = mm->mmap;
	unsigned long nr_accounted = 0;
	unsigned long end;

	/* mm's last user has gone, and its about to be pulled down */
	arch_exit_mmap(mm);

	lru_add_drain();
	flush_cache_mm(mm);
	tlb = tlb_gather_mmu(mm, 1);
	/* Don't update_hiwater_rss(mm) here, do_exit already did */
	/* Use -1 here to ensure all VMAs in the mm are unmapped */
	end = unmap_vmas(&tlb, vma, 0, -1, &nr_accounted, NULL);
	vm_unacct_memory(nr_accounted);
	free_pgtables(&tlb, vma, FIRST_USER_ADDRESS, 0);
	tlb_finish_mmu(tlb, 0, end);

	/*
	 * Walk the list again, actually closing and freeing it,
	 * with preemption enabled, without holding any MM locks.
	 */
	while (vma)
		vma = remove_vma(vma);

	BUG_ON(mm->nr_ptes > (FIRST_USER_ADDRESS+PMD_SIZE-1)>>PMD_SHIFT);
}