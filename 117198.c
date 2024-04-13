static void fuse_do_prepare_interrupt(fuse_req_t req, struct fuse_intr_data *d)
{
    d->id = pthread_self();
    pthread_cond_init(&d->cond, NULL);
    d->finished = 0;
    fuse_req_interrupt_func(req, fuse_interrupt, d);
}