static void fuse_finish_interrupt(struct fuse *f, fuse_req_t req,
                                         struct fuse_intr_data *d)
{
    if (f->conf.intr)
        fuse_do_finish_interrupt(f, req, d);
}