static pte_t make_huge_pte(struct vm_area_struct *vma, struct page *page,
				int writable)
{
	pte_t entry;
	unsigned int shift = huge_page_shift(hstate_vma(vma));

	if (writable) {
		entry = huge_pte_mkwrite(huge_pte_mkdirty(mk_huge_pte(page,
					 vma->vm_page_prot)));
	} else {
		entry = huge_pte_wrprotect(mk_huge_pte(page,
					   vma->vm_page_prot));
	}
	entry = pte_mkyoung(entry);
	entry = pte_mkhuge(entry);
	entry = arch_make_huge_pte(entry, shift, vma->vm_flags);

	return entry;
}