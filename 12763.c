static int smack_file_lock(struct file *file, unsigned int cmd)
{
	struct smk_audit_info ad;
	int rc;
	struct inode *inode = file_inode(file);

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, file->f_path);
	rc = smk_curacc(smk_of_inode(inode), MAY_LOCK, &ad);
	rc = smk_bu_file(file, MAY_LOCK, rc);
	return rc;
}