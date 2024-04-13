static inline int mm_counter(struct page *page)
{
	if (PageAnon(page))
		return MM_ANONPAGES;
	return mm_counter_file(page);
}