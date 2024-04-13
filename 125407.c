xfs_bulkstat_fmt(
	struct xfs_ibulk		*breq,
	const struct xfs_bulkstat	*bstat)
{
	if (copy_to_user(breq->ubuffer, bstat, sizeof(struct xfs_bulkstat)))
		return -EFAULT;
	return xfs_ibulk_advance(breq, sizeof(struct xfs_bulkstat));
}