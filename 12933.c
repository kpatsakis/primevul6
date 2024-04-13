static int smack_inode_removexattr(struct user_namespace *mnt_userns,
				   struct dentry *dentry, const char *name)
{
	struct inode_smack *isp;
	struct smk_audit_info ad;
	int rc = 0;

	if (strcmp(name, XATTR_NAME_SMACK) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKIPIN) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKIPOUT) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKEXEC) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKTRANSMUTE) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKMMAP) == 0) {
		if (!smack_privileged(CAP_MAC_ADMIN))
			rc = -EPERM;
	} else
		rc = cap_inode_removexattr(mnt_userns, dentry, name);

	if (rc != 0)
		return rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	rc = smk_curacc(smk_of_inode(d_backing_inode(dentry)), MAY_WRITE, &ad);
	rc = smk_bu_inode(d_backing_inode(dentry), MAY_WRITE, rc);
	if (rc != 0)
		return rc;

	isp = smack_inode(d_backing_inode(dentry));
	/*
	 * Don't do anything special for these.
	 *	XATTR_NAME_SMACKIPIN
	 *	XATTR_NAME_SMACKIPOUT
	 */
	if (strcmp(name, XATTR_NAME_SMACK) == 0) {
		struct super_block *sbp = dentry->d_sb;
		struct superblock_smack *sbsp = smack_superblock(sbp);

		isp->smk_inode = sbsp->smk_default;
	} else if (strcmp(name, XATTR_NAME_SMACKEXEC) == 0)
		isp->smk_task = NULL;
	else if (strcmp(name, XATTR_NAME_SMACKMMAP) == 0)
		isp->smk_mmap = NULL;
	else if (strcmp(name, XATTR_NAME_SMACKTRANSMUTE) == 0)
		isp->smk_flags &= ~SMK_INODE_TRANSMUTE;

	return 0;
}