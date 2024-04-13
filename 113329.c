static inline void cancel_dirty_page(struct page *page)
{
	/* Avoid atomic ops, locking, etc. when not actually needed. */
	if (PageDirty(page))
		__cancel_dirty_page(page);
}