static void lo_flock(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi,
                     int op)
{
    int res;
    (void)ino;

    res = flock(lo_fi_fd(req, fi), op);

    fuse_reply_err(req, res == -1 ? errno : 0);
}