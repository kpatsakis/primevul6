static void move_ptes(struct vm_area_struct *vma, pmd_t *old_pmd,
		unsigned long old_addr, unsigned long old_end,
		struct vm_area_struct *new_vma, pmd_t *new_pmd,
		unsigned long new_addr)
{
	struct address_space *mapping = NULL;
	struct mm_struct *mm = vma->vm_mm;
	pte_t *old_pte, *new_pte, pte;
	spinlock_t *old_ptl, *new_ptl;

	if (vma->vm_file) {
		/*
		 * Subtle point from Rajesh Venkatasubramanian: before
		 * moving file-based ptes, we must lock vmtruncate out,
		 * since it might clean the dst vma before the src vma,
		 * and we propagate stale pages into the dst afterward.
		 */
		mapping = vma->vm_file->f_mapping;
		spin_lock(&mapping->i_mmap_lock);
		if (new_vma->vm_truncate_count &&
		    new_vma->vm_truncate_count != vma->vm_truncate_count)
			new_vma->vm_truncate_count = 0;
	}

	/*
	 * We don't have to worry about the ordering of src and dst
	 * pte locks because exclusive mmap_sem prevents deadlock.
	 */
	old_pte = pte_offset_map_lock(mm, old_pmd, old_addr, &old_ptl);
 	new_pte = pte_offset_map_nested(new_pmd, new_addr);
	new_ptl = pte_lockptr(mm, new_pmd);
	if (new_ptl != old_ptl)
		spin_lock_nested(new_ptl, SINGLE_DEPTH_NESTING);
	arch_enter_lazy_mmu_mode();

	for (; old_addr < old_end; old_pte++, old_addr += PAGE_SIZE,
				   new_pte++, new_addr += PAGE_SIZE) {
		if (pte_none(*old_pte))
			continue;
		pte = ptep_clear_flush(vma, old_addr, old_pte);
		pte = move_pte(pte, new_vma->vm_page_prot, old_addr, new_addr);
		set_pte_at(mm, new_addr, new_pte, pte);
	}

	arch_leave_lazy_mmu_mode();
	if (new_ptl != old_ptl)
		spin_unlock(new_ptl);
	pte_unmap_nested(new_pte - 1);
	pte_unmap_unlock(old_pte - 1, old_ptl);
	if (mapping)
		spin_unlock(&mapping->i_mmap_lock);
}