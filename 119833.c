static void lo_fsyncdir(fuse_req_t req, fuse_ino_t ino, int datasync,
                        struct fuse_file_info *fi)
{
    int res;
    struct lo_dirp *d;
    int fd;

    (void)ino;

    d = lo_dirp(req, fi);
    if (!d) {
        fuse_reply_err(req, EBADF);
        return;
    }

    fd = dirfd(d->dp);
    if (datasync) {
        res = fdatasync(fd);
    } else {
        res = fsync(fd);
    }

    lo_dirp_put(&d);

    fuse_reply_err(req, res == -1 ? errno : 0);
}