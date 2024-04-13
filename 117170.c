int fuse_fs_ftruncate(struct fuse_fs *fs, const char *path, off_t size,
                      struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.ftruncate)
        return fs->op.ftruncate(path, size, fi);
    else if (fs->op.truncate)
        return fs->op.truncate(path, size);
    else
        return -ENOSYS;
}