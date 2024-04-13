static void unmap_region(struct mm_struct *mm,
		struct vm_area_struct *vma, struct vm_area_struct *prev,
		unsigned long start, unsigned long end)
{
	struct vm_area_struct *next = prev? prev->vm_next: mm->mmap;
	struct mmu_gather *tlb;
	unsigned long nr_accounted = 0;

	lru_add_drain();
	tlb = tlb_gather_mmu(mm, 0);
	update_hiwater_rss(mm);
	unmap_vmas(&tlb, vma, start, end, &nr_accounted, NULL);
	vm_unacct_memory(nr_accounted);
	free_pgtables(&tlb, vma, prev? prev->vm_end: FIRST_USER_ADDRESS,
				 next? next->vm_start: 0);
	tlb_finish_mmu(tlb, start, end);
}