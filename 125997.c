int copy_hugetlb_page_range(struct mm_struct *dst, struct mm_struct *src,
			    struct vm_area_struct *vma)
{
	pte_t *src_pte, *dst_pte, entry, dst_entry;
	struct page *ptepage;
	unsigned long addr;
	bool cow = is_cow_mapping(vma->vm_flags);
	struct hstate *h = hstate_vma(vma);
	unsigned long sz = huge_page_size(h);
	unsigned long npages = pages_per_huge_page(h);
	struct address_space *mapping = vma->vm_file->f_mapping;
	struct mmu_notifier_range range;
	int ret = 0;

	if (cow) {
		mmu_notifier_range_init(&range, MMU_NOTIFY_CLEAR, 0, vma, src,
					vma->vm_start,
					vma->vm_end);
		mmu_notifier_invalidate_range_start(&range);
	} else {
		/*
		 * For shared mappings i_mmap_rwsem must be held to call
		 * huge_pte_alloc, otherwise the returned ptep could go
		 * away if part of a shared pmd and another thread calls
		 * huge_pmd_unshare.
		 */
		i_mmap_lock_read(mapping);
	}

	for (addr = vma->vm_start; addr < vma->vm_end; addr += sz) {
		spinlock_t *src_ptl, *dst_ptl;
		src_pte = huge_pte_offset(src, addr, sz);
		if (!src_pte)
			continue;
		dst_pte = huge_pte_alloc(dst, vma, addr, sz);
		if (!dst_pte) {
			ret = -ENOMEM;
			break;
		}

		/*
		 * If the pagetables are shared don't copy or take references.
		 * dst_pte == src_pte is the common case of src/dest sharing.
		 *
		 * However, src could have 'unshared' and dst shares with
		 * another vma.  If dst_pte !none, this implies sharing.
		 * Check here before taking page table lock, and once again
		 * after taking the lock below.
		 */
		dst_entry = huge_ptep_get(dst_pte);
		if ((dst_pte == src_pte) || !huge_pte_none(dst_entry))
			continue;

		dst_ptl = huge_pte_lock(h, dst, dst_pte);
		src_ptl = huge_pte_lockptr(h, src, src_pte);
		spin_lock_nested(src_ptl, SINGLE_DEPTH_NESTING);
		entry = huge_ptep_get(src_pte);
		dst_entry = huge_ptep_get(dst_pte);
again:
		if (huge_pte_none(entry) || !huge_pte_none(dst_entry)) {
			/*
			 * Skip if src entry none.  Also, skip in the
			 * unlikely case dst entry !none as this implies
			 * sharing with another vma.
			 */
			;
		} else if (unlikely(is_hugetlb_entry_migration(entry) ||
				    is_hugetlb_entry_hwpoisoned(entry))) {
			swp_entry_t swp_entry = pte_to_swp_entry(entry);

			if (is_writable_migration_entry(swp_entry) && cow) {
				/*
				 * COW mappings require pages in both
				 * parent and child to be set to read.
				 */
				swp_entry = make_readable_migration_entry(
							swp_offset(swp_entry));
				entry = swp_entry_to_pte(swp_entry);
				set_huge_swap_pte_at(src, addr, src_pte,
						     entry, sz);
			}
			set_huge_swap_pte_at(dst, addr, dst_pte, entry, sz);
		} else {
			entry = huge_ptep_get(src_pte);
			ptepage = pte_page(entry);
			get_page(ptepage);

			/*
			 * This is a rare case where we see pinned hugetlb
			 * pages while they're prone to COW.  We need to do the
			 * COW earlier during fork.
			 *
			 * When pre-allocating the page or copying data, we
			 * need to be without the pgtable locks since we could
			 * sleep during the process.
			 */
			if (unlikely(page_needs_cow_for_dma(vma, ptepage))) {
				pte_t src_pte_old = entry;
				struct page *new;

				spin_unlock(src_ptl);
				spin_unlock(dst_ptl);
				/* Do not use reserve as it's private owned */
				new = alloc_huge_page(vma, addr, 1);
				if (IS_ERR(new)) {
					put_page(ptepage);
					ret = PTR_ERR(new);
					break;
				}
				copy_user_huge_page(new, ptepage, addr, vma,
						    npages);
				put_page(ptepage);

				/* Install the new huge page if src pte stable */
				dst_ptl = huge_pte_lock(h, dst, dst_pte);
				src_ptl = huge_pte_lockptr(h, src, src_pte);
				spin_lock_nested(src_ptl, SINGLE_DEPTH_NESTING);
				entry = huge_ptep_get(src_pte);
				if (!pte_same(src_pte_old, entry)) {
					restore_reserve_on_error(h, vma, addr,
								new);
					put_page(new);
					/* dst_entry won't change as in child */
					goto again;
				}
				hugetlb_install_page(vma, dst_pte, addr, new);
				spin_unlock(src_ptl);
				spin_unlock(dst_ptl);
				continue;
			}

			if (cow) {
				/*
				 * No need to notify as we are downgrading page
				 * table protection not changing it to point
				 * to a new page.
				 *
				 * See Documentation/vm/mmu_notifier.rst
				 */
				huge_ptep_set_wrprotect(src, addr, src_pte);
				entry = huge_pte_wrprotect(entry);
			}

			page_dup_rmap(ptepage, true);
			set_huge_pte_at(dst, addr, dst_pte, entry);
			hugetlb_count_add(npages, dst);
		}
		spin_unlock(src_ptl);
		spin_unlock(dst_ptl);
	}

	if (cow)
		mmu_notifier_invalidate_range_end(&range);
	else
		i_mmap_unlock_read(mapping);

	return ret;
}