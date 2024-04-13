static int smack_inode_rename(struct inode *old_inode,
			      struct dentry *old_dentry,
			      struct inode *new_inode,
			      struct dentry *new_dentry)
{
	int rc;
	struct smack_known *isp;
	struct smk_audit_info ad;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, old_dentry);

	isp = smk_of_inode(d_backing_inode(old_dentry));
	rc = smk_curacc(isp, MAY_READWRITE, &ad);
	rc = smk_bu_inode(d_backing_inode(old_dentry), MAY_READWRITE, rc);

	if (rc == 0 && d_is_positive(new_dentry)) {
		isp = smk_of_inode(d_backing_inode(new_dentry));
		smk_ad_setfield_u_fs_path_dentry(&ad, new_dentry);
		rc = smk_curacc(isp, MAY_READWRITE, &ad);
		rc = smk_bu_inode(d_backing_inode(new_dentry), MAY_READWRITE, rc);
	}
	return rc;
}