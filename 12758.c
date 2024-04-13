static int smack_file_open(struct file *file)
{
	struct task_smack *tsp = smack_cred(file->f_cred);
	struct inode *inode = file_inode(file);
	struct smk_audit_info ad;
	int rc;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, file->f_path);
	rc = smk_tskacc(tsp, smk_of_inode(inode), MAY_READ, &ad);
	rc = smk_bu_credfile(file->f_cred, file, MAY_READ, rc);

	return rc;
}