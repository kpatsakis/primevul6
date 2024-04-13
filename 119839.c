static void lo_fallocate(fuse_req_t req, fuse_ino_t ino, int mode, off_t offset,
                         off_t length, struct fuse_file_info *fi)
{
    int err = EOPNOTSUPP;
    (void)ino;

#ifdef CONFIG_FALLOCATE
    err = fallocate(lo_fi_fd(req, fi), mode, offset, length);
    if (err < 0) {
        err = errno;
    }

#elif defined(CONFIG_POSIX_FALLOCATE)
    if (mode) {
        fuse_reply_err(req, EOPNOTSUPP);
        return;
    }

    err = posix_fallocate(lo_fi_fd(req, fi), offset, length);
#endif

    fuse_reply_err(req, err);
}