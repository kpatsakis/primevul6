static inline bool ptlock_init(struct page *page)
{
	/*
	 * prep_new_page() initialize page->private (and therefore page->ptl)
	 * with 0. Make sure nobody took it in use in between.
	 *
	 * It can happen if arch try to use slab for page table allocation:
	 * slab code uses page->slab_cache, which share storage with page->ptl.
	 */
	VM_BUG_ON_PAGE(*(unsigned long *)&page->ptl, page);
	if (!ptlock_alloc(page))
		return false;
	spin_lock_init(ptlock_ptr(page));
	return true;
}