static inline void put_page(struct page *page)
{
	page = compound_head(page);

	/*
	 * For devmap managed pages we need to catch refcount transition from
	 * 2 to 1, when refcount reach one it means the page is free and we
	 * need to inform the device driver through callback. See
	 * include/linux/memremap.h and HMM for details.
	 */
	if (put_devmap_managed_page(page))
		return;

	if (put_page_testzero(page))
		__put_page(page);
}