static void update_and_free_pages_bulk(struct hstate *h, struct list_head *list)
{
	struct page *page, *t_page;

	list_for_each_entry_safe(page, t_page, list, lru) {
		update_and_free_page(h, page, false);
		cond_resched();
	}
}