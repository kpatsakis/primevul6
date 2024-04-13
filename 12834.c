static int __inode_security_revalidate(struct inode *inode,
				       struct dentry *dentry,
				       bool may_sleep)
{
	struct inode_security_struct *isec = selinux_inode(inode);

	might_sleep_if(may_sleep);

	if (selinux_initialized(&selinux_state) &&
	    isec->initialized != LABEL_INITIALIZED) {
		if (!may_sleep)
			return -ECHILD;

		/*
		 * Try reloading the inode security label.  This will fail if
		 * @opt_dentry is NULL and no dentry for this inode can be
		 * found; in that case, continue using the old label.
		 */
		inode_doinit_with_dentry(inode, dentry);
	}
	return 0;
}