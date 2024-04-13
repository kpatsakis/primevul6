static inline void unlock_or_release_subpool(struct hugepage_subpool *spool,
						unsigned long irq_flags)
{
	spin_unlock_irqrestore(&spool->lock, irq_flags);

	/* If no pages are used, and no other handles to the subpool
	 * remain, give up any reservations based on minimum size and
	 * free the subpool */
	if (subpool_is_free(spool)) {
		if (spool->min_hpages != -1)
			hugetlb_acct_memory(spool->hstate,
						-spool->min_hpages);
		kfree(spool);
	}
}