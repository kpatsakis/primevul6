hugetlb_install_page(struct vm_area_struct *vma, pte_t *ptep, unsigned long addr,
		     struct page *new_page)
{
	__SetPageUptodate(new_page);
	set_huge_pte_at(vma->vm_mm, addr, ptep, make_huge_pte(vma, new_page, 1));
	hugepage_add_new_anon_rmap(new_page, vma, addr);
	hugetlb_count_add(pages_per_huge_page(hstate_vma(vma)), vma->vm_mm);
	ClearHPageRestoreReserve(new_page);
	SetHPageMigratable(new_page);
}