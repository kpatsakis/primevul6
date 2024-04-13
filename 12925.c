static int smack_sb_statfs(struct dentry *dentry)
{
	struct superblock_smack *sbp = smack_superblock(dentry->d_sb);
	int rc;
	struct smk_audit_info ad;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	rc = smk_curacc(sbp->smk_floor, MAY_READ, &ad);
	rc = smk_bu_current("statfs", sbp->smk_floor, MAY_READ, rc);
	return rc;
}