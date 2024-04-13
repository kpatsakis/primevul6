static int fuse_flush_common(struct fuse *f, fuse_req_t req, fuse_ino_t ino,
                             const char *path, struct fuse_file_info *fi)
{
    struct fuse_intr_data d;
    struct flock lock;
    struct lock l;
    int err;
    int errlock;

    fuse_prepare_interrupt(f, req, &d);
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    err = fuse_fs_flush(f->fs, path, fi);
    errlock = fuse_fs_lock(f->fs, path, fi, F_SETLK, &lock);
    fuse_finish_interrupt(f, req, &d);

    if (errlock != -ENOSYS) {
        flock_to_lock(&lock, &l);
        l.owner = fi->lock_owner;
        pthread_mutex_lock(&f->lock);
        locks_insert(get_node(f, ino), &l);
        pthread_mutex_unlock(&f->lock);

        /* if op.lock() is defined FLUSH is needed regardless of op.flush() */
        if (err == -ENOSYS)
            err = 0;
    }
    return err;
}