static void fuse_lib_getxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
                              size_t size)
{
    struct fuse *f = req_fuse_prepare(req);
    int res;

    if (size) {
        char *value = (char *) malloc(size);
        if (value == NULL) {
            reply_err(req, -ENOMEM);
            return;
        }
        res = common_getxattr(f, req, ino, name, value, size);
        if (res > 0)
            fuse_reply_buf(req, value, res);
        else
            reply_err(req, res);
        free(value);
    } else {
        res = common_getxattr(f, req, ino, name, NULL, 0);
        if (res >= 0)
            fuse_reply_xattr(req, res);
        else
            reply_err(req, res);
    }
}