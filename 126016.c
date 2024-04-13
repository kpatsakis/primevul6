int huge_add_to_page_cache(struct page *page, struct address_space *mapping,
			   pgoff_t idx)
{
	struct inode *inode = mapping->host;
	struct hstate *h = hstate_inode(inode);
	int err = add_to_page_cache(page, mapping, idx, GFP_KERNEL);

	if (err)
		return err;
	ClearHPageRestoreReserve(page);

	/*
	 * set page dirty so that it will not be removed from cache/file
	 * by non-hugetlbfs specific code paths.
	 */
	set_page_dirty(page);

	spin_lock(&inode->i_lock);
	inode->i_blocks += blocks_per_huge_page(h);
	spin_unlock(&inode->i_lock);
	return 0;
}