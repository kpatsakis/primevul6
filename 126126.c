static void __init gather_bootmem_prealloc(void)
{
	struct huge_bootmem_page *m;

	list_for_each_entry(m, &huge_boot_pages, list) {
		struct page *page = virt_to_page(m);
		struct hstate *h = m->hstate;

		VM_BUG_ON(!hstate_is_gigantic(h));
		WARN_ON(page_count(page) != 1);
		if (prep_compound_gigantic_page(page, huge_page_order(h))) {
			WARN_ON(PageReserved(page));
			prep_new_huge_page(h, page, page_to_nid(page));
			put_page(page); /* add to the hugepage allocator */
		} else {
			/* VERY unlikely inflated ref count on a tail page */
			free_gigantic_page(page, huge_page_order(h));
		}

		/*
		 * We need to restore the 'stolen' pages to totalram_pages
		 * in order to fix confusing memory reports from free(1) and
		 * other side-effects, like CommitLimit going negative.
		 */
		adjust_managed_page_count(page, pages_per_huge_page(h));
		cond_resched();
	}
}