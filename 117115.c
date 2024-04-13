int fuse_fs_setxattr(struct fuse_fs *fs, const char *path, const char *name,
                     const char *value, size_t size, int flags)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.setxattr)
        return fs->op.setxattr(path, name, value, size, flags);
    else
        return -ENOSYS;
}