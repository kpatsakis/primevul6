int fuse_fs_symlink(struct fuse_fs *fs, const char *linkname, const char *path)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.symlink)
        return fs->op.symlink(linkname, path);
    else
        return -ENOSYS;
}