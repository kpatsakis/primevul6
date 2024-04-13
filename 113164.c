static inline int page_trans_huge_mapcount(struct page *page,
					   int *total_mapcount)
{
	int mapcount = page_mapcount(page);
	if (total_mapcount)
		*total_mapcount = mapcount;
	return mapcount;
}