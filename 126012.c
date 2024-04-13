void restore_reserve_on_error(struct hstate *h, struct vm_area_struct *vma,
			unsigned long address, struct page *page)
{
	long rc = vma_needs_reservation(h, vma, address);

	if (HPageRestoreReserve(page)) {
		if (unlikely(rc < 0))
			/*
			 * Rare out of memory condition in reserve map
			 * manipulation.  Clear HPageRestoreReserve so that
			 * global reserve count will not be incremented
			 * by free_huge_page.  This will make it appear
			 * as though the reservation for this page was
			 * consumed.  This may prevent the task from
			 * faulting in the page at a later time.  This
			 * is better than inconsistent global huge page
			 * accounting of reserve counts.
			 */
			ClearHPageRestoreReserve(page);
		else if (rc)
			(void)vma_add_reservation(h, vma, address);
		else
			vma_end_reservation(h, vma, address);
	} else {
		if (!rc) {
			/*
			 * This indicates there is an entry in the reserve map
			 * not added by alloc_huge_page.  We know it was added
			 * before the alloc_huge_page call, otherwise
			 * HPageRestoreReserve would be set on the page.
			 * Remove the entry so that a subsequent allocation
			 * does not consume a reservation.
			 */
			rc = vma_del_reservation(h, vma, address);
			if (rc < 0)
				/*
				 * VERY rare out of memory condition.  Since
				 * we can not delete the entry, set
				 * HPageRestoreReserve so that the reserve
				 * count will be incremented when the page
				 * is freed.  This reserve will be consumed
				 * on a subsequent allocation.
				 */
				SetHPageRestoreReserve(page);
		} else if (rc < 0) {
			/*
			 * Rare out of memory condition from
			 * vma_needs_reservation call.  Memory allocation is
			 * only attempted if a new entry is needed.  Therefore,
			 * this implies there is not an entry in the
			 * reserve map.
			 *
			 * For shared mappings, no entry in the map indicates
			 * no reservation.  We are done.
			 */
			if (!(vma->vm_flags & VM_MAYSHARE))
				/*
				 * For private mappings, no entry indicates
				 * a reservation is present.  Since we can
				 * not add an entry, set SetHPageRestoreReserve
				 * on the page so reserve count will be
				 * incremented when freed.  This reserve will
				 * be consumed on a subsequent allocation.
				 */
				SetHPageRestoreReserve(page);
		} else
			/*
			 * No reservation present, do nothing
			 */
			 vma_end_reservation(h, vma, address);
	}
}