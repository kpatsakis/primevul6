static void fuse_lib_setlk(fuse_req_t req, fuse_ino_t ino,
                           struct fuse_file_info *fi, struct flock *lock,
                           int should_sleep)
{
    int err = fuse_lock_common(req, ino, fi, lock, should_sleep ? F_SETLKW : F_SETLK);
    if (!err) {
        struct fuse *f = req_fuse(req);
        struct lock l;
        flock_to_lock(lock, &l);
        l.owner = fi->lock_owner;
        pthread_mutex_lock(&f->lock);
        locks_insert(get_node(f, ino), &l);
        pthread_mutex_unlock(&f->lock);
    }
    reply_err(req, err);
}