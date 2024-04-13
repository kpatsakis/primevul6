int fuse_fs_ioctl(struct fuse_fs *fs, const char *path, int cmd, void *arg,
		  struct fuse_file_info *fi, unsigned int flags, void *data)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.ioctl) {
/*
	if (fs->debug)
	    fprintf(stderr, "ioctl[%llu] 0x%x flags: 0x%x\n",
		    (unsigned long long) fi->fh, cmd, flags);
*/
	return fs->op.ioctl(path, cmd, arg, fi, flags, data);
    } else
	return -ENOSYS;
}