int fuse_fs_unlink(struct fuse_fs *fs, const char *path)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.unlink)
        return fs->op.unlink(path);
    else
        return -ENOSYS;
}