static inline u8 page_kasan_tag(const struct page *page)
{
	return (page->flags >> KASAN_TAG_PGSHIFT) & KASAN_TAG_MASK;
}