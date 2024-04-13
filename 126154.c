void hugetlb_unshare_all_pmds(struct vm_area_struct *vma)
{
	struct hstate *h = hstate_vma(vma);
	unsigned long sz = huge_page_size(h);
	struct mm_struct *mm = vma->vm_mm;
	struct mmu_notifier_range range;
	unsigned long address, start, end;
	spinlock_t *ptl;
	pte_t *ptep;

	if (!(vma->vm_flags & VM_MAYSHARE))
		return;

	start = ALIGN(vma->vm_start, PUD_SIZE);
	end = ALIGN_DOWN(vma->vm_end, PUD_SIZE);

	if (start >= end)
		return;

	/*
	 * No need to call adjust_range_if_pmd_sharing_possible(), because
	 * we have already done the PUD_SIZE alignment.
	 */
	mmu_notifier_range_init(&range, MMU_NOTIFY_CLEAR, 0, vma, mm,
				start, end);
	mmu_notifier_invalidate_range_start(&range);
	i_mmap_lock_write(vma->vm_file->f_mapping);
	for (address = start; address < end; address += PUD_SIZE) {
		unsigned long tmp = address;

		ptep = huge_pte_offset(mm, address, sz);
		if (!ptep)
			continue;
		ptl = huge_pte_lock(h, mm, ptep);
		/* We don't want 'address' to be changed */
		huge_pmd_unshare(mm, vma, &tmp, ptep);
		spin_unlock(ptl);
	}
	flush_hugetlb_tlb_range(vma, start, end);
	i_mmap_unlock_write(vma->vm_file->f_mapping);
	/*
	 * No need to call mmu_notifier_invalidate_range(), see
	 * Documentation/vm/mmu_notifier.rst.
	 */
	mmu_notifier_invalidate_range_end(&range);
}