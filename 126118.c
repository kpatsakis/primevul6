static bool __prep_compound_gigantic_page(struct page *page, unsigned int order,
								bool demote)
{
	int i, j;
	int nr_pages = 1 << order;
	struct page *p = page + 1;

	/* we rely on prep_new_huge_page to set the destructor */
	set_compound_order(page, order);
	__ClearPageReserved(page);
	__SetPageHead(page);
	for (i = 1; i < nr_pages; i++, p = mem_map_next(p, page, i)) {
		/*
		 * For gigantic hugepages allocated through bootmem at
		 * boot, it's safer to be consistent with the not-gigantic
		 * hugepages and clear the PG_reserved bit from all tail pages
		 * too.  Otherwise drivers using get_user_pages() to access tail
		 * pages may get the reference counting wrong if they see
		 * PG_reserved set on a tail page (despite the head page not
		 * having PG_reserved set).  Enforcing this consistency between
		 * head and tail pages allows drivers to optimize away a check
		 * on the head page when they need know if put_page() is needed
		 * after get_user_pages().
		 */
		__ClearPageReserved(p);
		/*
		 * Subtle and very unlikely
		 *
		 * Gigantic 'page allocators' such as memblock or cma will
		 * return a set of pages with each page ref counted.  We need
		 * to turn this set of pages into a compound page with tail
		 * page ref counts set to zero.  Code such as speculative page
		 * cache adding could take a ref on a 'to be' tail page.
		 * We need to respect any increased ref count, and only set
		 * the ref count to zero if count is currently 1.  If count
		 * is not 1, we return an error.  An error return indicates
		 * the set of pages can not be converted to a gigantic page.
		 * The caller who allocated the pages should then discard the
		 * pages using the appropriate free interface.
		 *
		 * In the case of demote, the ref count will be zero.
		 */
		if (!demote) {
			if (!page_ref_freeze(p, 1)) {
				pr_warn("HugeTLB page can not be used due to unexpected inflated ref count\n");
				goto out_error;
			}
		} else {
			VM_BUG_ON_PAGE(page_count(p), p);
		}
		set_compound_head(p, page);
	}
	atomic_set(compound_mapcount_ptr(page), -1);
	atomic_set(compound_pincount_ptr(page), 0);
	return true;

out_error:
	/* undo tail page modifications made above */
	p = page + 1;
	for (j = 1; j < i; j++, p = mem_map_next(p, page, j)) {
		clear_compound_head(p);
		set_page_refcounted(p);
	}
	/* need to clear PG_reserved on remaining tail pages  */
	for (; j < nr_pages; j++, p = mem_map_next(p, page, j))
		__ClearPageReserved(p);
	set_compound_order(page, 0);
	page[1].compound_nr = 0;
	__ClearPageHead(page);
	return false;
}