int fuse_fs_access(struct fuse_fs *fs, const char *path, int mask)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.access)
        return fs->op.access(path, mask);
    else
        return -ENOSYS;
}