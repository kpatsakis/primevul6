int fuse_fs_removexattr(struct fuse_fs *fs, const char *path, const char *name)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.removexattr)
        return fs->op.removexattr(path, name);
    else
        return -ENOSYS;
}