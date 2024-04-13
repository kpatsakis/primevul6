xfs_inumbers_fmt(
	struct xfs_ibulk		*breq,
	const struct xfs_inumbers	*igrp)
{
	if (copy_to_user(breq->ubuffer, igrp, sizeof(struct xfs_inumbers)))
		return -EFAULT;
	return xfs_ibulk_advance(breq, sizeof(struct xfs_inumbers));
}