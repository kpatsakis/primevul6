static vm_fault_t hugetlb_cow(struct mm_struct *mm, struct vm_area_struct *vma,
		       unsigned long address, pte_t *ptep,
		       struct page *pagecache_page, spinlock_t *ptl)
{
	pte_t pte;
	struct hstate *h = hstate_vma(vma);
	struct page *old_page, *new_page;
	int outside_reserve = 0;
	vm_fault_t ret = 0;
	unsigned long haddr = address & huge_page_mask(h);
	struct mmu_notifier_range range;

	pte = huge_ptep_get(ptep);
	old_page = pte_page(pte);

retry_avoidcopy:
	/* If no-one else is actually using this page, avoid the copy
	 * and just make the page writable */
	if (page_mapcount(old_page) == 1 && PageAnon(old_page)) {
		page_move_anon_rmap(old_page, vma);
		set_huge_ptep_writable(vma, haddr, ptep);
		return 0;
	}

	/*
	 * If the process that created a MAP_PRIVATE mapping is about to
	 * perform a COW due to a shared page count, attempt to satisfy
	 * the allocation without using the existing reserves. The pagecache
	 * page is used to determine if the reserve at this address was
	 * consumed or not. If reserves were used, a partial faulted mapping
	 * at the time of fork() could consume its reserves on COW instead
	 * of the full address range.
	 */
	if (is_vma_resv_set(vma, HPAGE_RESV_OWNER) &&
			old_page != pagecache_page)
		outside_reserve = 1;

	get_page(old_page);

	/*
	 * Drop page table lock as buddy allocator may be called. It will
	 * be acquired again before returning to the caller, as expected.
	 */
	spin_unlock(ptl);
	new_page = alloc_huge_page(vma, haddr, outside_reserve);

	if (IS_ERR(new_page)) {
		/*
		 * If a process owning a MAP_PRIVATE mapping fails to COW,
		 * it is due to references held by a child and an insufficient
		 * huge page pool. To guarantee the original mappers
		 * reliability, unmap the page from child processes. The child
		 * may get SIGKILLed if it later faults.
		 */
		if (outside_reserve) {
			struct address_space *mapping = vma->vm_file->f_mapping;
			pgoff_t idx;
			u32 hash;

			put_page(old_page);
			BUG_ON(huge_pte_none(pte));
			/*
			 * Drop hugetlb_fault_mutex and i_mmap_rwsem before
			 * unmapping.  unmapping needs to hold i_mmap_rwsem
			 * in write mode.  Dropping i_mmap_rwsem in read mode
			 * here is OK as COW mappings do not interact with
			 * PMD sharing.
			 *
			 * Reacquire both after unmap operation.
			 */
			idx = vma_hugecache_offset(h, vma, haddr);
			hash = hugetlb_fault_mutex_hash(mapping, idx);
			mutex_unlock(&hugetlb_fault_mutex_table[hash]);
			i_mmap_unlock_read(mapping);

			unmap_ref_private(mm, vma, old_page, haddr);

			i_mmap_lock_read(mapping);
			mutex_lock(&hugetlb_fault_mutex_table[hash]);
			spin_lock(ptl);
			ptep = huge_pte_offset(mm, haddr, huge_page_size(h));
			if (likely(ptep &&
				   pte_same(huge_ptep_get(ptep), pte)))
				goto retry_avoidcopy;
			/*
			 * race occurs while re-acquiring page table
			 * lock, and our job is done.
			 */
			return 0;
		}

		ret = vmf_error(PTR_ERR(new_page));
		goto out_release_old;
	}

	/*
	 * When the original hugepage is shared one, it does not have
	 * anon_vma prepared.
	 */
	if (unlikely(anon_vma_prepare(vma))) {
		ret = VM_FAULT_OOM;
		goto out_release_all;
	}

	copy_user_huge_page(new_page, old_page, address, vma,
			    pages_per_huge_page(h));
	__SetPageUptodate(new_page);

	mmu_notifier_range_init(&range, MMU_NOTIFY_CLEAR, 0, vma, mm, haddr,
				haddr + huge_page_size(h));
	mmu_notifier_invalidate_range_start(&range);

	/*
	 * Retake the page table lock to check for racing updates
	 * before the page tables are altered
	 */
	spin_lock(ptl);
	ptep = huge_pte_offset(mm, haddr, huge_page_size(h));
	if (likely(ptep && pte_same(huge_ptep_get(ptep), pte))) {
		ClearHPageRestoreReserve(new_page);

		/* Break COW */
		huge_ptep_clear_flush(vma, haddr, ptep);
		mmu_notifier_invalidate_range(mm, range.start, range.end);
		set_huge_pte_at(mm, haddr, ptep,
				make_huge_pte(vma, new_page, 1));
		page_remove_rmap(old_page, true);
		hugepage_add_new_anon_rmap(new_page, vma, haddr);
		SetHPageMigratable(new_page);
		/* Make the old page be freed below */
		new_page = old_page;
	}
	spin_unlock(ptl);
	mmu_notifier_invalidate_range_end(&range);
out_release_all:
	/* No restore in case of successful pagetable update (Break COW) */
	if (new_page != old_page)
		restore_reserve_on_error(h, vma, haddr, new_page);
	put_page(new_page);
out_release_old:
	put_page(old_page);

	spin_lock(ptl); /* Caller expects lock to be held */
	return ret;
}