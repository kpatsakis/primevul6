int fuse_fs_statfs(struct fuse_fs *fs, const char *path, struct statvfs *buf)
{
    fuse_get_context()->private_data = fs->user_data;
    if (fs->op.statfs)
        return fs->op.statfs(path, buf);
    else {
        buf->f_namemax = 255;
        buf->f_bsize = 512;
        return 0;
    }
}