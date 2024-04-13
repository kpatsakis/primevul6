static int smack_inode_unlink(struct inode *dir, struct dentry *dentry)
{
	struct inode *ip = d_backing_inode(dentry);
	struct smk_audit_info ad;
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	/*
	 * You need write access to the thing you're unlinking
	 */
	rc = smk_curacc(smk_of_inode(ip), MAY_WRITE, &ad);
	rc = smk_bu_inode(ip, MAY_WRITE, rc);
	if (rc == 0) {
		/*
		 * You also need write access to the containing directory
		 */
		smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_INODE);
		smk_ad_setfield_u_fs_inode(&ad, dir);
		rc = smk_curacc(smk_of_inode(dir), MAY_WRITE, &ad);
		rc = smk_bu_inode(dir, MAY_WRITE, rc);
	}
	return rc;
}