static void fuse_do_finish_interrupt(struct fuse *f, fuse_req_t req,
                                     struct fuse_intr_data *d)
{
    pthread_mutex_lock(&f->lock);
    d->finished = 1;
    pthread_cond_broadcast(&d->cond);
    pthread_mutex_unlock(&f->lock);
    fuse_req_interrupt_func(req, NULL, NULL);
    pthread_cond_destroy(&d->cond);
}