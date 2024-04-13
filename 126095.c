static long __vma_reservation_common(struct hstate *h,
				struct vm_area_struct *vma, unsigned long addr,
				enum vma_resv_mode mode)
{
	struct resv_map *resv;
	pgoff_t idx;
	long ret;
	long dummy_out_regions_needed;

	resv = vma_resv_map(vma);
	if (!resv)
		return 1;

	idx = vma_hugecache_offset(h, vma, addr);
	switch (mode) {
	case VMA_NEEDS_RESV:
		ret = region_chg(resv, idx, idx + 1, &dummy_out_regions_needed);
		/* We assume that vma_reservation_* routines always operate on
		 * 1 page, and that adding to resv map a 1 page entry can only
		 * ever require 1 region.
		 */
		VM_BUG_ON(dummy_out_regions_needed != 1);
		break;
	case VMA_COMMIT_RESV:
		ret = region_add(resv, idx, idx + 1, 1, NULL, NULL);
		/* region_add calls of range 1 should never fail. */
		VM_BUG_ON(ret < 0);
		break;
	case VMA_END_RESV:
		region_abort(resv, idx, idx + 1, 1);
		ret = 0;
		break;
	case VMA_ADD_RESV:
		if (vma->vm_flags & VM_MAYSHARE) {
			ret = region_add(resv, idx, idx + 1, 1, NULL, NULL);
			/* region_add calls of range 1 should never fail. */
			VM_BUG_ON(ret < 0);
		} else {
			region_abort(resv, idx, idx + 1, 1);
			ret = region_del(resv, idx, idx + 1);
		}
		break;
	case VMA_DEL_RESV:
		if (vma->vm_flags & VM_MAYSHARE) {
			region_abort(resv, idx, idx + 1, 1);
			ret = region_del(resv, idx, idx + 1);
		} else {
			ret = region_add(resv, idx, idx + 1, 1, NULL, NULL);
			/* region_add calls of range 1 should never fail. */
			VM_BUG_ON(ret < 0);
		}
		break;
	default:
		BUG();
	}

	if (vma->vm_flags & VM_MAYSHARE || mode == VMA_DEL_RESV)
		return ret;
	/*
	 * We know private mapping must have HPAGE_RESV_OWNER set.
	 *
	 * In most cases, reserves always exist for private mappings.
	 * However, a file associated with mapping could have been
	 * hole punched or truncated after reserves were consumed.
	 * As subsequent fault on such a range will not use reserves.
	 * Subtle - The reserve map for private mappings has the
	 * opposite meaning than that of shared mappings.  If NO
	 * entry is in the reserve map, it means a reservation exists.
	 * If an entry exists in the reserve map, it means the
	 * reservation has already been consumed.  As a result, the
	 * return value of this routine is the opposite of the
	 * value returned from reserve map manipulation routines above.
	 */
	if (ret > 0)
		return 0;
	if (ret == 0)
		return 1;
	return ret;
}