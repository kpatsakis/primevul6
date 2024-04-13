static void fuse_lib_forget(fuse_req_t req, fuse_ino_t ino,
                            unsigned long nlookup)
{
    struct fuse *f = req_fuse(req);
    if (f->conf.debug)
        fprintf(stderr, "FORGET %llu/%lu\n", (unsigned long long)ino, nlookup);
    forget_node(f, ino, nlookup);
    fuse_reply_none(req);
}