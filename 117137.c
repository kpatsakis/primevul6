int fuse_fs_read(struct fuse_fs *fs, const char *path, char *buf, size_t size,
                 off_t off, struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.read)
        return fs->op.read(path, buf, size, off, fi);
    else
        return -ENOSYS;
}