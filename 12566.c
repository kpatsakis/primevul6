static int smack_file_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	int rc = 0;
	struct smk_audit_info ad;
	struct inode *inode = file_inode(file);

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_PATH);
	smk_ad_setfield_u_fs_path(&ad, file->f_path);

	if (_IOC_DIR(cmd) & _IOC_WRITE) {
		rc = smk_curacc(smk_of_inode(inode), MAY_WRITE, &ad);
		rc = smk_bu_file(file, MAY_WRITE, rc);
	}

	if (rc == 0 && (_IOC_DIR(cmd) & _IOC_READ)) {
		rc = smk_curacc(smk_of_inode(inode), MAY_READ, &ad);
		rc = smk_bu_file(file, MAY_READ, rc);
	}

	return rc;
}