static int smack_inode_setxattr(struct user_namespace *mnt_userns,
				struct dentry *dentry, const char *name,
				const void *value, size_t size, int flags)
{
	struct smk_audit_info ad;
	struct smack_known *skp;
	int check_priv = 0;
	int check_import = 0;
	int check_star = 0;
	int rc = 0;

	/*
	 * Check label validity here so import won't fail in post_setxattr
	 */
	if (strcmp(name, XATTR_NAME_SMACK) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKIPIN) == 0 ||
	    strcmp(name, XATTR_NAME_SMACKIPOUT) == 0) {
		check_priv = 1;
		check_import = 1;
	} else if (strcmp(name, XATTR_NAME_SMACKEXEC) == 0 ||
		   strcmp(name, XATTR_NAME_SMACKMMAP) == 0) {
		check_priv = 1;
		check_import = 1;
		check_star = 1;
	} else if (strcmp(name, XATTR_NAME_SMACKTRANSMUTE) == 0) {
		check_priv = 1;
		if (size != TRANS_TRUE_SIZE ||
		    strncmp(value, TRANS_TRUE, TRANS_TRUE_SIZE) != 0)
			rc = -EINVAL;
	} else
		rc = cap_inode_setxattr(dentry, name, value, size, flags);

	if (check_priv && !smack_privileged(CAP_MAC_ADMIN))
		rc = -EPERM;

	if (rc == 0 && check_import) {
		skp = size ? smk_import_entry(value, size) : NULL;
		if (IS_ERR(skp))
			rc = PTR_ERR(skp);
		else if (skp == NULL || (check_star &&
		    (skp == &smack_known_star || skp == &smack_known_web)))
			rc = -EINVAL;
	}

	smk_ad_init(&ad, __func__, LSM_AUDIT_DATA_DENTRY);
	smk_ad_setfield_u_fs_path_dentry(&ad, dentry);

	if (rc == 0) {
		rc = smk_curacc(smk_of_inode(d_backing_inode(dentry)), MAY_WRITE, &ad);
		rc = smk_bu_inode(d_backing_inode(dentry), MAY_WRITE, rc);
	}

	return rc;
}