void arch_unmap_area_topdown(struct mm_struct *mm, unsigned long addr)
{
	/*
	 * Is this a new hole at the highest possible address?
	 */
	if (addr > mm->free_area_cache)
		mm->free_area_cache = addr;

	/* dont allow allocations above current base */
	if (mm->free_area_cache > mm->mmap_base)
		mm->free_area_cache = mm->mmap_base;
}