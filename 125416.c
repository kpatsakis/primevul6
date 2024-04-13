xfs_find_handle(
	unsigned int		cmd,
	xfs_fsop_handlereq_t	*hreq)
{
	int			hsize;
	xfs_handle_t		handle;
	struct inode		*inode;
	struct fd		f = {NULL};
	struct path		path;
	int			error;
	struct xfs_inode	*ip;

	if (cmd == XFS_IOC_FD_TO_HANDLE) {
		f = fdget(hreq->fd);
		if (!f.file)
			return -EBADF;
		inode = file_inode(f.file);
	} else {
		error = user_path_at(AT_FDCWD, hreq->path, 0, &path);
		if (error)
			return error;
		inode = d_inode(path.dentry);
	}
	ip = XFS_I(inode);

	/*
	 * We can only generate handles for inodes residing on a XFS filesystem,
	 * and only for regular files, directories or symbolic links.
	 */
	error = -EINVAL;
	if (inode->i_sb->s_magic != XFS_SB_MAGIC)
		goto out_put;

	error = -EBADF;
	if (!S_ISREG(inode->i_mode) &&
	    !S_ISDIR(inode->i_mode) &&
	    !S_ISLNK(inode->i_mode))
		goto out_put;


	memcpy(&handle.ha_fsid, ip->i_mount->m_fixedfsid, sizeof(xfs_fsid_t));

	if (cmd == XFS_IOC_PATH_TO_FSHANDLE) {
		/*
		 * This handle only contains an fsid, zero the rest.
		 */
		memset(&handle.ha_fid, 0, sizeof(handle.ha_fid));
		hsize = sizeof(xfs_fsid_t);
	} else {
		handle.ha_fid.fid_len = sizeof(xfs_fid_t) -
					sizeof(handle.ha_fid.fid_len);
		handle.ha_fid.fid_pad = 0;
		handle.ha_fid.fid_gen = inode->i_generation;
		handle.ha_fid.fid_ino = ip->i_ino;
		hsize = sizeof(xfs_handle_t);
	}

	error = -EFAULT;
	if (copy_to_user(hreq->ohandle, &handle, hsize) ||
	    copy_to_user(hreq->ohandlen, &hsize, sizeof(__s32)))
		goto out_put;

	error = 0;

 out_put:
	if (cmd == XFS_IOC_FD_TO_HANDLE)
		fdput(f);
	else
		path_put(&path);
	return error;
}