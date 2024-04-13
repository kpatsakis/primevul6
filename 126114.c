void clear_vma_resv_huge_pages(struct vm_area_struct *vma)
{
	/*
	 * Clear the old hugetlb private page reservation.
	 * It has already been transferred to new_vma.
	 *
	 * During a mremap() operation of a hugetlb vma we call move_vma()
	 * which copies vma into new_vma and unmaps vma. After the copy
	 * operation both new_vma and vma share a reference to the resv_map
	 * struct, and at that point vma is about to be unmapped. We don't
	 * want to return the reservation to the pool at unmap of vma because
	 * the reservation still lives on in new_vma, so simply decrement the
	 * ref here and remove the resv_map reference from this vma.
	 */
	struct resv_map *reservations = vma_resv_map(vma);

	if (reservations && is_vma_resv_set(vma, HPAGE_RESV_OWNER)) {
		resv_map_put_hugetlb_cgroup_uncharge_info(reservations);
		kref_put(&reservations->refs, resv_map_release);
	}

	reset_vma_resv_huge_pages(vma);
}