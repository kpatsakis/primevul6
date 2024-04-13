int fuse_fs_fsync(struct fuse_fs *fs, const char *path, int datasync,
                  struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.fsync)
        return fs->op.fsync(path, datasync, fi);
    else
        return -ENOSYS;
}