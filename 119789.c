static void lo_fsync(fuse_req_t req, fuse_ino_t ino, int datasync,
                     struct fuse_file_info *fi)
{
    int res;
    int fd;
    char *buf;

    fuse_log(FUSE_LOG_DEBUG, "lo_fsync(ino=%" PRIu64 ", fi=0x%p)\n", ino,
             (void *)fi);

    if (!fi) {
        struct lo_data *lo = lo_data(req);

        res = asprintf(&buf, "%i", lo_fd(req, ino));
        if (res == -1) {
            return (void)fuse_reply_err(req, errno);
        }

        fd = openat(lo->proc_self_fd, buf, O_RDWR);
        free(buf);
        if (fd == -1) {
            return (void)fuse_reply_err(req, errno);
        }
    } else {
        fd = lo_fi_fd(req, fi);
    }

    if (datasync) {
        res = fdatasync(fd);
    } else {
        res = fsync(fd);
    }
    if (!fi) {
        close(fd);
    }
    fuse_reply_err(req, res == -1 ? errno : 0);
}