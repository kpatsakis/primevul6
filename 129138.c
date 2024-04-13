struct file *open_exec(const char *name)
{
	struct nameidata nd;
	int err;
	struct file *file;

	err = path_lookup_open(AT_FDCWD, name, LOOKUP_FOLLOW, &nd, FMODE_READ|FMODE_EXEC);
	file = ERR_PTR(err);

	if (!err) {
		struct inode *inode = nd.dentry->d_inode;
		file = ERR_PTR(-EACCES);
		if (!(nd.mnt->mnt_flags & MNT_NOEXEC) &&
		    S_ISREG(inode->i_mode)) {
			int err = vfs_permission(&nd, MAY_EXEC);
			file = ERR_PTR(err);
			if (!err) {
				file = nameidata_to_filp(&nd, O_RDONLY);
				if (!IS_ERR(file)) {
					err = deny_write_access(file);
					if (err) {
						fput(file);
						file = ERR_PTR(err);
					}
				}
out:
				return file;
			}
		}
		release_open_intent(&nd);
		path_release(&nd);
	}
	goto out;
}