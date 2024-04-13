static void smack_inode_post_setxattr(struct dentry *dentry, const char *name,
				      const void *value, size_t size, int flags)
{
	struct smack_known *skp;
	struct inode_smack *isp = smack_inode(d_backing_inode(dentry));

	if (strcmp(name, XATTR_NAME_SMACKTRANSMUTE) == 0) {
		isp->smk_flags |= SMK_INODE_TRANSMUTE;
		return;
	}

	if (strcmp(name, XATTR_NAME_SMACK) == 0) {
		skp = smk_import_entry(value, size);
		if (!IS_ERR(skp))
			isp->smk_inode = skp;
	} else if (strcmp(name, XATTR_NAME_SMACKEXEC) == 0) {
		skp = smk_import_entry(value, size);
		if (!IS_ERR(skp))
			isp->smk_task = skp;
	} else if (strcmp(name, XATTR_NAME_SMACKMMAP) == 0) {
		skp = smk_import_entry(value, size);
		if (!IS_ERR(skp))
			isp->smk_mmap = skp;
	}

	return;
}