int fuse_fs_chown(struct fuse_fs *fs, const char *path, uid_t uid, gid_t gid)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.chown)
        return fs->op.chown(path, uid, gid);
    else
        return -ENOSYS;
}