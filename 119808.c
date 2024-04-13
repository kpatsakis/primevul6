static void lo_statfs(fuse_req_t req, fuse_ino_t ino)
{
    int res;
    struct statvfs stbuf;

    res = fstatvfs(lo_fd(req, ino), &stbuf);
    if (res == -1) {
        fuse_reply_err(req, errno);
    } else {
        fuse_reply_statfs(req, &stbuf);
    }
}