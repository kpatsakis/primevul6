int fuse_fs_rmdir(struct fuse_fs *fs, const char *path)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.rmdir)
        return fs->op.rmdir(path);
    else
        return -ENOSYS;
}