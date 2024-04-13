static void fuse_interrupt(fuse_req_t req, void *d_)
{
    struct fuse_intr_data *d = d_;
    struct fuse *f = req_fuse(req);

    if (d->id == pthread_self())
        return;

    pthread_mutex_lock(&f->lock);
    while (!d->finished) {
        struct timeval now;
        struct timespec timeout;

        pthread_kill(d->id, f->conf.intr_signal);
        gettimeofday(&now, NULL);
        timeout.tv_sec = now.tv_sec + 1;
        timeout.tv_nsec = now.tv_usec * 1000;
        pthread_cond_timedwait(&d->cond, &f->lock, &timeout);
    }
    pthread_mutex_unlock(&f->lock);
}