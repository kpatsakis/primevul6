static vm_fault_t hugetlb_no_page(struct mm_struct *mm,
			struct vm_area_struct *vma,
			struct address_space *mapping, pgoff_t idx,
			unsigned long address, pte_t *ptep, unsigned int flags)
{
	struct hstate *h = hstate_vma(vma);
	vm_fault_t ret = VM_FAULT_SIGBUS;
	int anon_rmap = 0;
	unsigned long size;
	struct page *page;
	pte_t new_pte;
	spinlock_t *ptl;
	unsigned long haddr = address & huge_page_mask(h);
	bool new_page, new_pagecache_page = false;

	/*
	 * Currently, we are forced to kill the process in the event the
	 * original mapper has unmapped pages from the child due to a failed
	 * COW. Warn that such a situation has occurred as it may not be obvious
	 */
	if (is_vma_resv_set(vma, HPAGE_RESV_UNMAPPED)) {
		pr_warn_ratelimited("PID %d killed due to inadequate hugepage pool\n",
			   current->pid);
		return ret;
	}

	/*
	 * We can not race with truncation due to holding i_mmap_rwsem.
	 * i_size is modified when holding i_mmap_rwsem, so check here
	 * once for faults beyond end of file.
	 */
	size = i_size_read(mapping->host) >> huge_page_shift(h);
	if (idx >= size)
		goto out;

retry:
	new_page = false;
	page = find_lock_page(mapping, idx);
	if (!page) {
		/* Check for page in userfault range */
		if (userfaultfd_missing(vma)) {
			ret = hugetlb_handle_userfault(vma, mapping, idx,
						       flags, haddr,
						       VM_UFFD_MISSING);
			goto out;
		}

		page = alloc_huge_page(vma, haddr, 0);
		if (IS_ERR(page)) {
			/*
			 * Returning error will result in faulting task being
			 * sent SIGBUS.  The hugetlb fault mutex prevents two
			 * tasks from racing to fault in the same page which
			 * could result in false unable to allocate errors.
			 * Page migration does not take the fault mutex, but
			 * does a clear then write of pte's under page table
			 * lock.  Page fault code could race with migration,
			 * notice the clear pte and try to allocate a page
			 * here.  Before returning error, get ptl and make
			 * sure there really is no pte entry.
			 */
			ptl = huge_pte_lock(h, mm, ptep);
			ret = 0;
			if (huge_pte_none(huge_ptep_get(ptep)))
				ret = vmf_error(PTR_ERR(page));
			spin_unlock(ptl);
			goto out;
		}
		clear_huge_page(page, address, pages_per_huge_page(h));
		__SetPageUptodate(page);
		new_page = true;

		if (vma->vm_flags & VM_MAYSHARE) {
			int err = huge_add_to_page_cache(page, mapping, idx);
			if (err) {
				put_page(page);
				if (err == -EEXIST)
					goto retry;
				goto out;
			}
			new_pagecache_page = true;
		} else {
			lock_page(page);
			if (unlikely(anon_vma_prepare(vma))) {
				ret = VM_FAULT_OOM;
				goto backout_unlocked;
			}
			anon_rmap = 1;
		}
	} else {
		/*
		 * If memory error occurs between mmap() and fault, some process
		 * don't have hwpoisoned swap entry for errored virtual address.
		 * So we need to block hugepage fault by PG_hwpoison bit check.
		 */
		if (unlikely(PageHWPoison(page))) {
			ret = VM_FAULT_HWPOISON_LARGE |
				VM_FAULT_SET_HINDEX(hstate_index(h));
			goto backout_unlocked;
		}

		/* Check for page in userfault range. */
		if (userfaultfd_minor(vma)) {
			unlock_page(page);
			put_page(page);
			ret = hugetlb_handle_userfault(vma, mapping, idx,
						       flags, haddr,
						       VM_UFFD_MINOR);
			goto out;
		}
	}

	/*
	 * If we are going to COW a private mapping later, we examine the
	 * pending reservations for this page now. This will ensure that
	 * any allocations necessary to record that reservation occur outside
	 * the spinlock.
	 */
	if ((flags & FAULT_FLAG_WRITE) && !(vma->vm_flags & VM_SHARED)) {
		if (vma_needs_reservation(h, vma, haddr) < 0) {
			ret = VM_FAULT_OOM;
			goto backout_unlocked;
		}
		/* Just decrements count, does not deallocate */
		vma_end_reservation(h, vma, haddr);
	}

	ptl = huge_pte_lock(h, mm, ptep);
	ret = 0;
	if (!huge_pte_none(huge_ptep_get(ptep)))
		goto backout;

	if (anon_rmap) {
		ClearHPageRestoreReserve(page);
		hugepage_add_new_anon_rmap(page, vma, haddr);
	} else
		page_dup_rmap(page, true);
	new_pte = make_huge_pte(vma, page, ((vma->vm_flags & VM_WRITE)
				&& (vma->vm_flags & VM_SHARED)));
	set_huge_pte_at(mm, haddr, ptep, new_pte);

	hugetlb_count_add(pages_per_huge_page(h), mm);
	if ((flags & FAULT_FLAG_WRITE) && !(vma->vm_flags & VM_SHARED)) {
		/* Optimization, do the COW without a second fault */
		ret = hugetlb_cow(mm, vma, address, ptep, page, ptl);
	}

	spin_unlock(ptl);

	/*
	 * Only set HPageMigratable in newly allocated pages.  Existing pages
	 * found in the pagecache may not have HPageMigratableset if they have
	 * been isolated for migration.
	 */
	if (new_page)
		SetHPageMigratable(page);

	unlock_page(page);
out:
	return ret;

backout:
	spin_unlock(ptl);
backout_unlocked:
	unlock_page(page);
	/* restore reserve for newly allocated pages not in page cache */
	if (new_page && !new_pagecache_page)
		restore_reserve_on_error(h, vma, haddr, page);
	put_page(page);
	goto out;
}