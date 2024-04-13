int fuse_fs_opendir(struct fuse_fs *fs, const char *path,
                    struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.opendir)
        return fs->op.opendir(path, fi);
    else
        return 0;
}