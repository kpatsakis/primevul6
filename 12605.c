static struct smack_known *smk_fetch(const char *name, struct inode *ip,
					struct dentry *dp)
{
	int rc;
	char *buffer;
	struct smack_known *skp = NULL;

	if (!(ip->i_opflags & IOP_XATTR))
		return ERR_PTR(-EOPNOTSUPP);

	buffer = kzalloc(SMK_LONGLABEL, GFP_NOFS);
	if (buffer == NULL)
		return ERR_PTR(-ENOMEM);

	rc = __vfs_getxattr(dp, ip, name, buffer, SMK_LONGLABEL);
	if (rc < 0)
		skp = ERR_PTR(rc);
	else if (rc == 0)
		skp = NULL;
	else
		skp = smk_import_entry(buffer, rc);

	kfree(buffer);

	return skp;
}