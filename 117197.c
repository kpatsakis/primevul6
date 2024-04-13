int fuse_fs_chmod(struct fuse_fs *fs, const char *path, mode_t mode)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.chmod)
        return fs->op.chmod(path, mode);
    else
        return -ENOSYS;
}