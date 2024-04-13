static inline int mm_counter_file(struct page *page)
{
	if (PageSwapBacked(page))
		return MM_SHMEMPAGES;
	return MM_FILEPAGES;
}