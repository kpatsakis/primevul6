static void fuse_prepare_interrupt(struct fuse *f, fuse_req_t req,
                                          struct fuse_intr_data *d)
{
    if (f->conf.intr)
        fuse_do_prepare_interrupt(req, d);
}