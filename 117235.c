int fuse_fs_fsyncdir(struct fuse_fs *fs, const char *path, int datasync,
                     struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.fsyncdir)
        return fs->op.fsyncdir(path, datasync, fi);
    else
        return -ENOSYS;
}