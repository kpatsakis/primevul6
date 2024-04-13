xfs_fsinumbers_fmt(
	struct xfs_ibulk		*breq,
	const struct xfs_inumbers	*igrp)
{
	struct xfs_inogrp		ig1;

	xfs_inumbers_to_inogrp(&ig1, igrp);
	if (copy_to_user(breq->ubuffer, &ig1, sizeof(struct xfs_inogrp)))
		return -EFAULT;
	return xfs_ibulk_advance(breq, sizeof(struct xfs_inogrp));
}