static void reply_entry(fuse_req_t req, const struct fuse_entry_param *e,
                        int err)
{
    if (!err) {
        struct fuse *f = req_fuse(req);
#ifdef __SOLARIS__
                     /* Skip forget for negative result */
        if ((fuse_reply_entry(req, e) == -ENOENT)
          && (e->ino != 0))
            forget_node(f, e->ino, 1);
#else /* __SOLARIS__ */
        if (fuse_reply_entry(req, e) == -ENOENT)
            forget_node(f, e->ino, 1);
#endif
    } else
        reply_err(req, err);
}