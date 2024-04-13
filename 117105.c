int fuse_fs_mkdir(struct fuse_fs *fs, const char *path, mode_t mode)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.mkdir)
        return fs->op.mkdir(path, mode);
    else
        return -ENOSYS;
}