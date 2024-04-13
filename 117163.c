int fuse_fs_rename(struct fuse_fs *fs, const char *oldpath,
                   const char *newpath)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.rename)
        return fs->op.rename(oldpath, newpath);
    else
        return -ENOSYS;
}