int fuse_fs_write(struct fuse_fs *fs, const char *path, const char *buf,
                  size_t size, off_t off, struct fuse_file_info *fi)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.write)
        return fs->op.write(path, buf, size, off, fi);
    else
        return -ENOSYS;
}