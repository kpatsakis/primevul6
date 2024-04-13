static long vma_del_reservation(struct hstate *h,
			struct vm_area_struct *vma, unsigned long addr)
{
	return __vma_reservation_common(h, vma, addr, VMA_DEL_RESV);
}