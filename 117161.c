static void fuse_lib_getlk(fuse_req_t req, fuse_ino_t ino,
                           struct fuse_file_info *fi, struct flock *lock)
{
    int err;
    struct lock l;
    struct lock *conflict;
    struct fuse *f = req_fuse(req);

    flock_to_lock(lock, &l);
    l.owner = fi->lock_owner;
    pthread_mutex_lock(&f->lock);
    conflict = locks_conflict(get_node(f, ino), &l);
    if (conflict)
        lock_to_flock(conflict, lock);
    pthread_mutex_unlock(&f->lock);
    if (!conflict)
        err = fuse_lock_common(req, ino, fi, lock, F_GETLK);
    else
        err = 0;

    if (!err)
        fuse_reply_lock(req, lock);
    else
        reply_err(req, err);
}