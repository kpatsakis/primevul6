xfs_ioc_getlabel(
	struct xfs_mount	*mp,
	char			__user *user_label)
{
	struct xfs_sb		*sbp = &mp->m_sb;
	char			label[XFSLABEL_MAX + 1];

	/* Paranoia */
	BUILD_BUG_ON(sizeof(sbp->sb_fname) > FSLABEL_MAX);

	/* 1 larger than sb_fname, so this ensures a trailing NUL char */
	memset(label, 0, sizeof(label));
	spin_lock(&mp->m_sb_lock);
	strncpy(label, sbp->sb_fname, XFSLABEL_MAX);
	spin_unlock(&mp->m_sb_lock);

	if (copy_to_user(user_label, label, sizeof(label)))
		return -EFAULT;
	return 0;
}