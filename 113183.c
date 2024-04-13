static inline void page_kasan_tag_reset(struct page *page)
{
	page_kasan_tag_set(page, 0xff);
}