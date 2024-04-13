static struct page *alloc_buddy_huge_page(struct hstate *h,
		gfp_t gfp_mask, int nid, nodemask_t *nmask,
		nodemask_t *node_alloc_noretry)
{
	int order = huge_page_order(h);
	struct page *page;
	bool alloc_try_hard = true;

	/*
	 * By default we always try hard to allocate the page with
	 * __GFP_RETRY_MAYFAIL flag.  However, if we are allocating pages in
	 * a loop (to adjust global huge page counts) and previous allocation
	 * failed, do not continue to try hard on the same node.  Use the
	 * node_alloc_noretry bitmap to manage this state information.
	 */
	if (node_alloc_noretry && node_isset(nid, *node_alloc_noretry))
		alloc_try_hard = false;
	gfp_mask |= __GFP_COMP|__GFP_NOWARN;
	if (alloc_try_hard)
		gfp_mask |= __GFP_RETRY_MAYFAIL;
	if (nid == NUMA_NO_NODE)
		nid = numa_mem_id();
	page = __alloc_pages(gfp_mask, order, nid, nmask);
	if (page)
		__count_vm_event(HTLB_BUDDY_PGALLOC);
	else
		__count_vm_event(HTLB_BUDDY_PGALLOC_FAIL);

	/*
	 * If we did not specify __GFP_RETRY_MAYFAIL, but still got a page this
	 * indicates an overall state change.  Clear bit so that we resume
	 * normal 'try hard' allocations.
	 */
	if (node_alloc_noretry && page && !alloc_try_hard)
		node_clear(nid, *node_alloc_noretry);

	/*
	 * If we tried hard to get a page but failed, set bit so that
	 * subsequent attempts will not try as hard until there is an
	 * overall state change.
	 */
	if (node_alloc_noretry && !page && alloc_try_hard)
		node_set(nid, *node_alloc_noretry);

	return page;
}