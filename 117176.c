int fuse_fs_getxattr(struct fuse_fs *fs, const char *path, const char *name,
                     char *value, size_t size)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.getxattr)
        return fs->op.getxattr(path, name, value, size);
    else
        return -ENOSYS;
}