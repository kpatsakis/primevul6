struct page *alloc_buddy_huge_page_with_mpol(struct hstate *h,
		struct vm_area_struct *vma, unsigned long addr)
{
	struct page *page = NULL;
	struct mempolicy *mpol;
	gfp_t gfp_mask = htlb_alloc_mask(h);
	int nid;
	nodemask_t *nodemask;

	nid = huge_node(vma, addr, gfp_mask, &mpol, &nodemask);
	if (mpol_is_preferred_many(mpol)) {
		gfp_t gfp = gfp_mask | __GFP_NOWARN;

		gfp &=  ~(__GFP_DIRECT_RECLAIM | __GFP_NOFAIL);
		page = alloc_surplus_huge_page(h, gfp, nid, nodemask, false);

		/* Fallback to all nodes if page==NULL */
		nodemask = NULL;
	}

	if (!page)
		page = alloc_surplus_huge_page(h, gfp_mask, nid, nodemask, false);
	mpol_cond_put(mpol);
	return page;
}