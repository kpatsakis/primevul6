static int smack_set_mnt_opts(struct super_block *sb,
		void *mnt_opts,
		unsigned long kern_flags,
		unsigned long *set_kern_flags)
{
	struct dentry *root = sb->s_root;
	struct inode *inode = d_backing_inode(root);
	struct superblock_smack *sp = smack_superblock(sb);
	struct inode_smack *isp;
	struct smack_known *skp;
	struct smack_mnt_opts *opts = mnt_opts;
	bool transmute = false;

	if (sp->smk_flags & SMK_SB_INITIALIZED)
		return 0;

	if (inode->i_security == NULL) {
		int rc = lsm_inode_alloc(inode);

		if (rc)
			return rc;
	}

	if (!smack_privileged(CAP_MAC_ADMIN)) {
		/*
		 * Unprivileged mounts don't get to specify Smack values.
		 */
		if (opts)
			return -EPERM;
		/*
		 * Unprivileged mounts get root and default from the caller.
		 */
		skp = smk_of_current();
		sp->smk_root = skp;
		sp->smk_default = skp;
		/*
		 * For a handful of fs types with no user-controlled
		 * backing store it's okay to trust security labels
		 * in the filesystem. The rest are untrusted.
		 */
		if (sb->s_user_ns != &init_user_ns &&
		    sb->s_magic != SYSFS_MAGIC && sb->s_magic != TMPFS_MAGIC &&
		    sb->s_magic != RAMFS_MAGIC) {
			transmute = true;
			sp->smk_flags |= SMK_SB_UNTRUSTED;
		}
	}

	sp->smk_flags |= SMK_SB_INITIALIZED;

	if (opts) {
		if (opts->fsdefault) {
			skp = smk_import_entry(opts->fsdefault, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_default = skp;
		}
		if (opts->fsfloor) {
			skp = smk_import_entry(opts->fsfloor, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_floor = skp;
		}
		if (opts->fshat) {
			skp = smk_import_entry(opts->fshat, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_hat = skp;
		}
		if (opts->fsroot) {
			skp = smk_import_entry(opts->fsroot, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_root = skp;
		}
		if (opts->fstransmute) {
			skp = smk_import_entry(opts->fstransmute, 0);
			if (IS_ERR(skp))
				return PTR_ERR(skp);
			sp->smk_root = skp;
			transmute = true;
		}
	}

	/*
	 * Initialize the root inode.
	 */
	init_inode_smack(inode, sp->smk_root);

	if (transmute) {
		isp = smack_inode(inode);
		isp->smk_flags |= SMK_INODE_TRANSMUTE;
	}

	return 0;
}