static inline void page_kasan_tag_set(struct page *page, u8 tag)
{
	page->flags &= ~(KASAN_TAG_MASK << KASAN_TAG_PGSHIFT);
	page->flags |= (tag & KASAN_TAG_MASK) << KASAN_TAG_PGSHIFT;
}