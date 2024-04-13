static void lo_readlink(fuse_req_t req, fuse_ino_t ino)
{
    char buf[PATH_MAX + 1];
    int res;

    res = readlinkat(lo_fd(req, ino), "", buf, sizeof(buf));
    if (res == -1) {
        return (void)fuse_reply_err(req, errno);
    }

    if (res == sizeof(buf)) {
        return (void)fuse_reply_err(req, ENAMETOOLONG);
    }

    buf[res] = '\0';

    fuse_reply_readlink(req, buf);
}