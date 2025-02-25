static int sb_finish_set_opts(struct super_block *sb)
{
	struct superblock_security_struct *sbsec = selinux_superblock(sb);
	struct dentry *root = sb->s_root;
	struct inode *root_inode = d_backing_inode(root);
	int rc = 0;

	if (sbsec->behavior == SECURITY_FS_USE_XATTR) {
		rc = sb_check_xattr_support(sb);
		if (rc)
			return rc;
	}

	sbsec->flags |= SE_SBINITIALIZED;

	/*
	 * Explicitly set or clear SBLABEL_MNT.  It's not sufficient to simply
	 * leave the flag untouched because sb_clone_mnt_opts might be handing
	 * us a superblock that needs the flag to be cleared.
	 */
	if (selinux_is_sblabel_mnt(sb))
		sbsec->flags |= SBLABEL_MNT;
	else
		sbsec->flags &= ~SBLABEL_MNT;

	/* Initialize the root inode. */
	rc = inode_doinit_with_dentry(root_inode, root);

	/* Initialize any other inodes associated with the superblock, e.g.
	   inodes created prior to initial policy load or inodes created
	   during get_sb by a pseudo filesystem that directly
	   populates itself. */
	spin_lock(&sbsec->isec_lock);
	while (!list_empty(&sbsec->isec_head)) {
		struct inode_security_struct *isec =
				list_first_entry(&sbsec->isec_head,
					   struct inode_security_struct, list);
		struct inode *inode = isec->inode;
		list_del_init(&isec->list);
		spin_unlock(&sbsec->isec_lock);
		inode = igrab(inode);
		if (inode) {
			if (!IS_PRIVATE(inode))
				inode_doinit_with_dentry(inode, NULL);
			iput(inode);
		}
		spin_lock(&sbsec->isec_lock);
	}
	spin_unlock(&sbsec->isec_lock);
	return rc;
}