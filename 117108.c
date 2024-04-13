int fuse_fs_utimens(struct fuse_fs *fs, const char *path,
                    const struct timespec tv[2])
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.utimens)
        return fs->op.utimens(path, tv);
    else if(fs->op.utime) {
        struct utimbuf buf;
        buf.actime = tv[0].tv_sec;
        buf.modtime = tv[1].tv_sec;
        return fs->op.utime(path, &buf);
    } else
        return -ENOSYS;
}