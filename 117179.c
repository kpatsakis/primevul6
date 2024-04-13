int fuse_fs_truncate(struct fuse_fs *fs, const char *path, off_t size)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.truncate)
        return fs->op.truncate(path, size);
    else
        return -ENOSYS;
}