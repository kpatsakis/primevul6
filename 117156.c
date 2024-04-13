int fuse_fs_readlink(struct fuse_fs *fs, const char *path, char *buf,
                     size_t len)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.readlink)
        return fs->op.readlink(path, buf, len);
    else
        return -ENOSYS;
}