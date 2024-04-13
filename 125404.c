xfs_attrlist_by_handle(
	struct file		*parfilp,
	struct xfs_fsop_attrlist_handlereq __user *p)
{
	struct xfs_fsop_attrlist_handlereq al_hreq;
	struct dentry		*dentry;
	int			error = -ENOMEM;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	if (copy_from_user(&al_hreq, p, sizeof(al_hreq)))
		return -EFAULT;

	dentry = xfs_handlereq_to_dentry(parfilp, &al_hreq.hreq);
	if (IS_ERR(dentry))
		return PTR_ERR(dentry);

	error = xfs_ioc_attr_list(XFS_I(d_inode(dentry)), al_hreq.buffer,
				  al_hreq.buflen, al_hreq.flags, &p->pos);
	dput(dentry);
	return error;
}