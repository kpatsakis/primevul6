static void lo_getattr(fuse_req_t req, fuse_ino_t ino,
                       struct fuse_file_info *fi)
{
    int res;
    struct stat buf;
    struct lo_data *lo = lo_data(req);

    (void)fi;

    res =
        fstatat(lo_fd(req, ino), "", &buf, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
    if (res == -1) {
        return (void)fuse_reply_err(req, errno);
    }

    fuse_reply_attr(req, &buf, lo->timeout);
}