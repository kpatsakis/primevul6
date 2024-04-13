unsigned long hugetlb_change_protection(struct vm_area_struct *vma,
		unsigned long address, unsigned long end, pgprot_t newprot)
{
	struct mm_struct *mm = vma->vm_mm;
	unsigned long start = address;
	pte_t *ptep;
	pte_t pte;
	struct hstate *h = hstate_vma(vma);
	unsigned long pages = 0;
	bool shared_pmd = false;
	struct mmu_notifier_range range;

	/*
	 * In the case of shared PMDs, the area to flush could be beyond
	 * start/end.  Set range.start/range.end to cover the maximum possible
	 * range if PMD sharing is possible.
	 */
	mmu_notifier_range_init(&range, MMU_NOTIFY_PROTECTION_VMA,
				0, vma, mm, start, end);
	adjust_range_if_pmd_sharing_possible(vma, &range.start, &range.end);

	BUG_ON(address >= end);
	flush_cache_range(vma, range.start, range.end);

	mmu_notifier_invalidate_range_start(&range);
	i_mmap_lock_write(vma->vm_file->f_mapping);
	for (; address < end; address += huge_page_size(h)) {
		spinlock_t *ptl;
		ptep = huge_pte_offset(mm, address, huge_page_size(h));
		if (!ptep)
			continue;
		ptl = huge_pte_lock(h, mm, ptep);
		if (huge_pmd_unshare(mm, vma, &address, ptep)) {
			pages++;
			spin_unlock(ptl);
			shared_pmd = true;
			continue;
		}
		pte = huge_ptep_get(ptep);
		if (unlikely(is_hugetlb_entry_hwpoisoned(pte))) {
			spin_unlock(ptl);
			continue;
		}
		if (unlikely(is_hugetlb_entry_migration(pte))) {
			swp_entry_t entry = pte_to_swp_entry(pte);

			if (is_writable_migration_entry(entry)) {
				pte_t newpte;

				entry = make_readable_migration_entry(
							swp_offset(entry));
				newpte = swp_entry_to_pte(entry);
				set_huge_swap_pte_at(mm, address, ptep,
						     newpte, huge_page_size(h));
				pages++;
			}
			spin_unlock(ptl);
			continue;
		}
		if (!huge_pte_none(pte)) {
			pte_t old_pte;
			unsigned int shift = huge_page_shift(hstate_vma(vma));

			old_pte = huge_ptep_modify_prot_start(vma, address, ptep);
			pte = pte_mkhuge(huge_pte_modify(old_pte, newprot));
			pte = arch_make_huge_pte(pte, shift, vma->vm_flags);
			huge_ptep_modify_prot_commit(vma, address, ptep, old_pte, pte);
			pages++;
		}
		spin_unlock(ptl);
	}
	/*
	 * Must flush TLB before releasing i_mmap_rwsem: x86's huge_pmd_unshare
	 * may have cleared our pud entry and done put_page on the page table:
	 * once we release i_mmap_rwsem, another task can do the final put_page
	 * and that page table be reused and filled with junk.  If we actually
	 * did unshare a page of pmds, flush the range corresponding to the pud.
	 */
	if (shared_pmd)
		flush_hugetlb_tlb_range(vma, range.start, range.end);
	else
		flush_hugetlb_tlb_range(vma, start, end);
	/*
	 * No need to call mmu_notifier_invalidate_range() we are downgrading
	 * page table protection not changing it to point to a new page.
	 *
	 * See Documentation/vm/mmu_notifier.rst
	 */
	i_mmap_unlock_write(vma->vm_file->f_mapping);
	mmu_notifier_invalidate_range_end(&range);

	return pages << h->order;
}