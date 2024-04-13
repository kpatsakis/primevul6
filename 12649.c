static int smack_inode_getxattr(struct dentry *dentry, const char *name)
{
	struct smk_audit_info ad;
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	rc = smk_curacc(smk_of_inode(d_backing_inode(dentry)), MAY_READ, &ad);
	rc = smk_bu_inode(d_backing_inode(dentry), MAY_READ, rc);
	return rc;
}