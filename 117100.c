int fuse_fs_create(struct fuse_fs *fs, const char *path, mode_t mode,
                   struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.create)
        return fs->op.create(path, mode, fi);
    else
        return -ENOSYS;
}