static int smack_file_fcntl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	struct smk_audit_info ad;
	int rc = 0;
	struct inode *inode = file_inode(file);

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	switch (cmd) {
	case F_GETLK:
		break;
	case F_SETLK:
	case F_SETLKW:
		smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
		smk_ad_setfield_u_fs_path(&ad, file->f_path);
		rc = smk_curacc(smk_of_inode(inode), MAY_LOCK, &ad);
		rc = smk_bu_file(file, MAY_LOCK, rc);
		break;
	case F_SETOWN:
	case F_SETSIG:
		smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
		smk_ad_setfield_u_fs_path(&ad, file->f_path);
		rc = smk_curacc(smk_of_inode(inode), MAY_WRITE, &ad);
		rc = smk_bu_file(file, MAY_WRITE, rc);
		break;
	default:
		break;
	}

	return rc;
}