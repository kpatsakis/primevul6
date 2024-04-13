void hugetlb_fix_reserve_counts(struct inode *inode)
{
	struct hugepage_subpool *spool = subpool_inode(inode);
	long rsv_adjust;
	bool reserved = false;

	rsv_adjust = hugepage_subpool_get_pages(spool, 1);
	if (rsv_adjust > 0) {
		struct hstate *h = hstate_inode(inode);

		if (!hugetlb_acct_memory(h, 1))
			reserved = true;
	} else if (!rsv_adjust) {
		reserved = true;
	}

	if (!reserved)
		pr_warn("hugetlb: Huge Page Reserved count may go negative.\n");
}