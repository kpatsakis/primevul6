int fuse_fs_lock(struct fuse_fs *fs, const char *path,
                 struct fuse_file_info *fi, int cmd, struct flock *lock)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.lock)
        return fs->op.lock(path, fi, cmd, lock);
    else
        return -ENOSYS;
}