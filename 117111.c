int fuse_fs_listxattr(struct fuse_fs *fs, const char *path, char *list,
                      size_t size)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.listxattr)
        return fs->op.listxattr(path, list, size);
    else
        return -ENOSYS;
}