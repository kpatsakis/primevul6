xfs_fsbulkstat_one_fmt(
	struct xfs_ibulk		*breq,
	const struct xfs_bulkstat	*bstat)
{
	struct xfs_bstat		bs1;

	xfs_bulkstat_to_bstat(breq->mp, &bs1, bstat);
	if (copy_to_user(breq->ubuffer, &bs1, sizeof(bs1)))
		return -EFAULT;
	return xfs_ibulk_advance(breq, sizeof(struct xfs_bstat));
}