int fuse_fs_flush(struct fuse_fs *fs, const char *path,
                  struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.flush)
        return fs->op.flush(path, fi);
    else
        return -ENOSYS;
}