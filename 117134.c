int fuse_fs_fgetattr(struct fuse_fs *fs, const char *path, struct stat *buf,
                     struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.fgetattr)
        return fs->op.fgetattr(path, buf, fi);
    else if (fs->op.getattr)
        return fs->op.getattr(path, buf);
    else
        return -ENOSYS;
}