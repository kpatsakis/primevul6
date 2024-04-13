void unmap_hugepage_range(struct vm_area_struct *vma, unsigned long start,
			  unsigned long end, struct page *ref_page)
{
	struct mmu_gather tlb;

	tlb_gather_mmu(&tlb, vma->vm_mm);
	__unmap_hugepage_range(&tlb, vma, start, end, ref_page);
	tlb_finish_mmu(&tlb);
}