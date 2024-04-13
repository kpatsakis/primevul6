static struct page *alloc_fresh_huge_page(struct hstate *h,
		gfp_t gfp_mask, int nid, nodemask_t *nmask,
		nodemask_t *node_alloc_noretry)
{
	struct page *page;
	bool retry = false;

retry:
	if (hstate_is_gigantic(h))
		page = alloc_gigantic_page(h, gfp_mask, nid, nmask);
	else
		page = alloc_buddy_huge_page(h, gfp_mask,
				nid, nmask, node_alloc_noretry);
	if (!page)
		return NULL;

	if (hstate_is_gigantic(h)) {
		if (!prep_compound_gigantic_page(page, huge_page_order(h))) {
			/*
			 * Rare failure to convert pages to compound page.
			 * Free pages and try again - ONCE!
			 */
			free_gigantic_page(page, huge_page_order(h));
			if (!retry) {
				retry = true;
				goto retry;
			}
			return NULL;
		}
	}
	prep_new_huge_page(h, page, page_to_nid(page));

	return page;
}