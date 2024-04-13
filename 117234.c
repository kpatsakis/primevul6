int fuse_fs_readdir(struct fuse_fs *fs, const char *path, void *buf,
                    fuse_fill_dir_t filler, off_t off,
                    struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.readdir)
        return fs->op.readdir(path, buf, filler, off, fi);
    else
        return -ENOSYS;
}