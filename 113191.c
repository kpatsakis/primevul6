static inline bool page_is_guard(struct page *page)
{
	struct page_ext *page_ext;

	if (!debug_guardpage_enabled())
		return false;

	page_ext = lookup_page_ext(page);
	if (unlikely(!page_ext))
		return false;

	return test_bit(PAGE_EXT_DEBUG_GUARD, &page_ext->flags);
}