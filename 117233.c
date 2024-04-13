int fuse_fs_getattr(struct fuse_fs *fs, const char *path, struct stat *buf)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.getattr)
        return fs->op.getattr(path, buf);
    else
        return -ENOSYS;
}