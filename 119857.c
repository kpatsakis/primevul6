static void lo_lseek(fuse_req_t req, fuse_ino_t ino, off_t off, int whence,
                     struct fuse_file_info *fi)
{
    off_t res;

    (void)ino;
    res = lseek(lo_fi_fd(req, fi), off, whence);
    if (res != -1) {
        fuse_reply_lseek(req, res);
    } else {
        fuse_reply_err(req, errno);
    }
}