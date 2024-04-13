int fuse_fs_mknod(struct fuse_fs *fs, const char *path, mode_t mode,
                  dev_t rdev)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.mknod)
        return fs->op.mknod(path, mode, rdev);
    else
        return -ENOSYS;
}