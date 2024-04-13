int fuse_fs_releasedir(struct fuse_fs *fs, const char *path,
                       struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.releasedir)
        return fs->op.releasedir(path, fi);
    else
        return 0;
}