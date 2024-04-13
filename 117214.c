int fuse_fs_link(struct fuse_fs *fs, const char *oldpath, const char *newpath)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.link)
        return fs->op.link(oldpath, newpath);
    else
        return -ENOSYS;
}