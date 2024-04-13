void hugepage_put_subpool(struct hugepage_subpool *spool)
{
	unsigned long flags;

	spin_lock_irqsave(&spool->lock, flags);
	BUG_ON(!spool->count);
	spool->count--;
	unlock_or_release_subpool(spool, flags);
}