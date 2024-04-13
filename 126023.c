int hugetlb_mcopy_atomic_pte(struct mm_struct *dst_mm,
			    pte_t *dst_pte,
			    struct vm_area_struct *dst_vma,
			    unsigned long dst_addr,
			    unsigned long src_addr,
			    enum mcopy_atomic_mode mode,
			    struct page **pagep)
{
	bool is_continue = (mode == MCOPY_ATOMIC_CONTINUE);
	struct hstate *h = hstate_vma(dst_vma);
	struct address_space *mapping = dst_vma->vm_file->f_mapping;
	pgoff_t idx = vma_hugecache_offset(h, dst_vma, dst_addr);
	unsigned long size;
	int vm_shared = dst_vma->vm_flags & VM_SHARED;
	pte_t _dst_pte;
	spinlock_t *ptl;
	int ret = -ENOMEM;
	struct page *page;
	int writable;
	bool page_in_pagecache = false;

	if (is_continue) {
		ret = -EFAULT;
		page = find_lock_page(mapping, idx);
		if (!page)
			goto out;
		page_in_pagecache = true;
	} else if (!*pagep) {
		/* If a page already exists, then it's UFFDIO_COPY for
		 * a non-missing case. Return -EEXIST.
		 */
		if (vm_shared &&
		    hugetlbfs_pagecache_present(h, dst_vma, dst_addr)) {
			ret = -EEXIST;
			goto out;
		}

		page = alloc_huge_page(dst_vma, dst_addr, 0);
		if (IS_ERR(page)) {
			ret = -ENOMEM;
			goto out;
		}

		ret = copy_huge_page_from_user(page,
						(const void __user *) src_addr,
						pages_per_huge_page(h), false);

		/* fallback to copy_from_user outside mmap_lock */
		if (unlikely(ret)) {
			ret = -ENOENT;
			/* Free the allocated page which may have
			 * consumed a reservation.
			 */
			restore_reserve_on_error(h, dst_vma, dst_addr, page);
			put_page(page);

			/* Allocate a temporary page to hold the copied
			 * contents.
			 */
			page = alloc_huge_page_vma(h, dst_vma, dst_addr);
			if (!page) {
				ret = -ENOMEM;
				goto out;
			}
			*pagep = page;
			/* Set the outparam pagep and return to the caller to
			 * copy the contents outside the lock. Don't free the
			 * page.
			 */
			goto out;
		}
	} else {
		if (vm_shared &&
		    hugetlbfs_pagecache_present(h, dst_vma, dst_addr)) {
			put_page(*pagep);
			ret = -EEXIST;
			*pagep = NULL;
			goto out;
		}

		page = alloc_huge_page(dst_vma, dst_addr, 0);
		if (IS_ERR(page)) {
			ret = -ENOMEM;
			*pagep = NULL;
			goto out;
		}
		folio_copy(page_folio(page), page_folio(*pagep));
		put_page(*pagep);
		*pagep = NULL;
	}

	/*
	 * The memory barrier inside __SetPageUptodate makes sure that
	 * preceding stores to the page contents become visible before
	 * the set_pte_at() write.
	 */
	__SetPageUptodate(page);

	/* Add shared, newly allocated pages to the page cache. */
	if (vm_shared && !is_continue) {
		size = i_size_read(mapping->host) >> huge_page_shift(h);
		ret = -EFAULT;
		if (idx >= size)
			goto out_release_nounlock;

		/*
		 * Serialization between remove_inode_hugepages() and
		 * huge_add_to_page_cache() below happens through the
		 * hugetlb_fault_mutex_table that here must be hold by
		 * the caller.
		 */
		ret = huge_add_to_page_cache(page, mapping, idx);
		if (ret)
			goto out_release_nounlock;
		page_in_pagecache = true;
	}

	ptl = huge_pte_lockptr(h, dst_mm, dst_pte);
	spin_lock(ptl);

	/*
	 * Recheck the i_size after holding PT lock to make sure not
	 * to leave any page mapped (as page_mapped()) beyond the end
	 * of the i_size (remove_inode_hugepages() is strict about
	 * enforcing that). If we bail out here, we'll also leave a
	 * page in the radix tree in the vm_shared case beyond the end
	 * of the i_size, but remove_inode_hugepages() will take care
	 * of it as soon as we drop the hugetlb_fault_mutex_table.
	 */
	size = i_size_read(mapping->host) >> huge_page_shift(h);
	ret = -EFAULT;
	if (idx >= size)
		goto out_release_unlock;

	ret = -EEXIST;
	if (!huge_pte_none(huge_ptep_get(dst_pte)))
		goto out_release_unlock;

	if (vm_shared) {
		page_dup_rmap(page, true);
	} else {
		ClearHPageRestoreReserve(page);
		hugepage_add_new_anon_rmap(page, dst_vma, dst_addr);
	}

	/* For CONTINUE on a non-shared VMA, don't set VM_WRITE for CoW. */
	if (is_continue && !vm_shared)
		writable = 0;
	else
		writable = dst_vma->vm_flags & VM_WRITE;

	_dst_pte = make_huge_pte(dst_vma, page, writable);
	if (writable)
		_dst_pte = huge_pte_mkdirty(_dst_pte);
	_dst_pte = pte_mkyoung(_dst_pte);

	set_huge_pte_at(dst_mm, dst_addr, dst_pte, _dst_pte);

	(void)huge_ptep_set_access_flags(dst_vma, dst_addr, dst_pte, _dst_pte,
					dst_vma->vm_flags & VM_WRITE);
	hugetlb_count_add(pages_per_huge_page(h), dst_mm);

	/* No need to invalidate - it was non-present before */
	update_mmu_cache(dst_vma, dst_addr, dst_pte);

	spin_unlock(ptl);
	if (!is_continue)
		SetHPageMigratable(page);
	if (vm_shared || is_continue)
		unlock_page(page);
	ret = 0;
out:
	return ret;
out_release_unlock:
	spin_unlock(ptl);
	if (vm_shared || is_continue)
		unlock_page(page);
out_release_nounlock:
	if (!page_in_pagecache)
		restore_reserve_on_error(h, dst_vma, dst_addr, page);
	put_page(page);
	goto out;
}