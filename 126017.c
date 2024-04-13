static void move_huge_pte(struct vm_area_struct *vma, unsigned long old_addr,
			  unsigned long new_addr, pte_t *src_pte)
{
	struct hstate *h = hstate_vma(vma);
	struct mm_struct *mm = vma->vm_mm;
	pte_t *dst_pte, pte;
	spinlock_t *src_ptl, *dst_ptl;

	dst_pte = huge_pte_offset(mm, new_addr, huge_page_size(h));
	dst_ptl = huge_pte_lock(h, mm, dst_pte);
	src_ptl = huge_pte_lockptr(h, mm, src_pte);

	/*
	 * We don't have to worry about the ordering of src and dst ptlocks
	 * because exclusive mmap_sem (or the i_mmap_lock) prevents deadlock.
	 */
	if (src_ptl != dst_ptl)
		spin_lock_nested(src_ptl, SINGLE_DEPTH_NESTING);

	pte = huge_ptep_get_and_clear(mm, old_addr, src_pte);
	set_huge_pte_at(mm, new_addr, dst_pte, pte);

	if (src_ptl != dst_ptl)
		spin_unlock(src_ptl);
	spin_unlock(dst_ptl);
}