long follow_hugetlb_page(struct mm_struct *mm, struct vm_area_struct *vma,
			 struct page **pages, struct vm_area_struct **vmas,
			 unsigned long *position, unsigned long *nr_pages,
			 long i, unsigned int flags, int *locked)
{
	unsigned long pfn_offset;
	unsigned long vaddr = *position;
	unsigned long remainder = *nr_pages;
	struct hstate *h = hstate_vma(vma);
	int err = -EFAULT, refs;

	while (vaddr < vma->vm_end && remainder) {
		pte_t *pte;
		spinlock_t *ptl = NULL;
		int absent;
		struct page *page;

		/*
		 * If we have a pending SIGKILL, don't keep faulting pages and
		 * potentially allocating memory.
		 */
		if (fatal_signal_pending(current)) {
			remainder = 0;
			break;
		}

		/*
		 * Some archs (sparc64, sh*) have multiple pte_ts to
		 * each hugepage.  We have to make sure we get the
		 * first, for the page indexing below to work.
		 *
		 * Note that page table lock is not held when pte is null.
		 */
		pte = huge_pte_offset(mm, vaddr & huge_page_mask(h),
				      huge_page_size(h));
		if (pte)
			ptl = huge_pte_lock(h, mm, pte);
		absent = !pte || huge_pte_none(huge_ptep_get(pte));

		/*
		 * When coredumping, it suits get_dump_page if we just return
		 * an error where there's an empty slot with no huge pagecache
		 * to back it.  This way, we avoid allocating a hugepage, and
		 * the sparse dumpfile avoids allocating disk blocks, but its
		 * huge holes still show up with zeroes where they need to be.
		 */
		if (absent && (flags & FOLL_DUMP) &&
		    !hugetlbfs_pagecache_present(h, vma, vaddr)) {
			if (pte)
				spin_unlock(ptl);
			remainder = 0;
			break;
		}

		/*
		 * We need call hugetlb_fault for both hugepages under migration
		 * (in which case hugetlb_fault waits for the migration,) and
		 * hwpoisoned hugepages (in which case we need to prevent the
		 * caller from accessing to them.) In order to do this, we use
		 * here is_swap_pte instead of is_hugetlb_entry_migration and
		 * is_hugetlb_entry_hwpoisoned. This is because it simply covers
		 * both cases, and because we can't follow correct pages
		 * directly from any kind of swap entries.
		 */
		if (absent || is_swap_pte(huge_ptep_get(pte)) ||
		    ((flags & FOLL_WRITE) &&
		      !huge_pte_write(huge_ptep_get(pte)))) {
			vm_fault_t ret;
			unsigned int fault_flags = 0;

			if (pte)
				spin_unlock(ptl);
			if (flags & FOLL_WRITE)
				fault_flags |= FAULT_FLAG_WRITE;
			if (locked)
				fault_flags |= FAULT_FLAG_ALLOW_RETRY |
					FAULT_FLAG_KILLABLE;
			if (flags & FOLL_NOWAIT)
				fault_flags |= FAULT_FLAG_ALLOW_RETRY |
					FAULT_FLAG_RETRY_NOWAIT;
			if (flags & FOLL_TRIED) {
				/*
				 * Note: FAULT_FLAG_ALLOW_RETRY and
				 * FAULT_FLAG_TRIED can co-exist
				 */
				fault_flags |= FAULT_FLAG_TRIED;
			}
			ret = hugetlb_fault(mm, vma, vaddr, fault_flags);
			if (ret & VM_FAULT_ERROR) {
				err = vm_fault_to_errno(ret, flags);
				remainder = 0;
				break;
			}
			if (ret & VM_FAULT_RETRY) {
				if (locked &&
				    !(fault_flags & FAULT_FLAG_RETRY_NOWAIT))
					*locked = 0;
				*nr_pages = 0;
				/*
				 * VM_FAULT_RETRY must not return an
				 * error, it will return zero
				 * instead.
				 *
				 * No need to update "position" as the
				 * caller will not check it after
				 * *nr_pages is set to 0.
				 */
				return i;
			}
			continue;
		}

		pfn_offset = (vaddr & ~huge_page_mask(h)) >> PAGE_SHIFT;
		page = pte_page(huge_ptep_get(pte));

		/*
		 * If subpage information not requested, update counters
		 * and skip the same_page loop below.
		 */
		if (!pages && !vmas && !pfn_offset &&
		    (vaddr + huge_page_size(h) < vma->vm_end) &&
		    (remainder >= pages_per_huge_page(h))) {
			vaddr += huge_page_size(h);
			remainder -= pages_per_huge_page(h);
			i += pages_per_huge_page(h);
			spin_unlock(ptl);
			continue;
		}

		/* vaddr may not be aligned to PAGE_SIZE */
		refs = min3(pages_per_huge_page(h) - pfn_offset, remainder,
		    (vma->vm_end - ALIGN_DOWN(vaddr, PAGE_SIZE)) >> PAGE_SHIFT);

		if (pages || vmas)
			record_subpages_vmas(mem_map_offset(page, pfn_offset),
					     vma, refs,
					     likely(pages) ? pages + i : NULL,
					     vmas ? vmas + i : NULL);

		if (pages) {
			/*
			 * try_grab_compound_head() should always succeed here,
			 * because: a) we hold the ptl lock, and b) we've just
			 * checked that the huge page is present in the page
			 * tables. If the huge page is present, then the tail
			 * pages must also be present. The ptl prevents the
			 * head page and tail pages from being rearranged in
			 * any way. So this page must be available at this
			 * point, unless the page refcount overflowed:
			 */
			if (WARN_ON_ONCE(!try_grab_compound_head(pages[i],
								 refs,
								 flags))) {
				spin_unlock(ptl);
				remainder = 0;
				err = -ENOMEM;
				break;
			}
		}

		vaddr += (refs << PAGE_SHIFT);
		remainder -= refs;
		i += refs;

		spin_unlock(ptl);
	}
	*nr_pages = remainder;
	/*
	 * setting position is actually required only if remainder is
	 * not zero but it's faster not to add a "if (remainder)"
	 * branch.
	 */
	*position = vaddr;

	return i ? i : err;
}