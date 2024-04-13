static int smack_inode_getattr(const struct path *path)
{
	struct smk_audit_info ad;
	struct inode *inode = d_backing_inode(path->dentry);
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, *path);
	rc = smk_curacc(smk_of_inode(inode), MAY_READ, &ad);
	rc = smk_bu_inode(inode, MAY_READ, rc);
	return rc;
}