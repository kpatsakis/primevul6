static void fuse_lib_open(fuse_req_t req, fuse_ino_t ino,
                          struct fuse_file_info *fi)
{
    struct fuse *f = req_fuse_prepare(req);
    struct fuse_intr_data d;
    char *path = NULL;
    int err = 0;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path) {
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_open(f->fs, path, fi);
        if (!err) {
            if (f->conf.direct_io)
                fi->direct_io = 1;
            if (f->conf.kernel_cache)
                fi->keep_cache = 1;
#ifdef __SOLARIS__

            if (f->conf.auto_cache)
                open_auto_cache(f, ino, path, fi);
#endif /* __SOLARIS__ */
        }
        fuse_finish_interrupt(f, req, &d);
    }
    if (!err) {
        pthread_mutex_lock(&f->lock);
        get_node(f, ino)->open_count++;
        pthread_mutex_unlock(&f->lock);
        if (fuse_reply_open(req, fi) == -ENOENT) {
            /* The open syscall was interrupted, so it must be cancelled */
            fuse_prepare_interrupt(f, req, &d);
            fuse_do_release(f, ino, path, fi);
            fuse_finish_interrupt(f, req, &d);
        } else if (f->conf.debug) {
            fprintf(stderr, "OPEN[%llu] flags: 0x%x %s\n",
                    (unsigned long long) fi->fh, fi->flags, path);
        }
    } else
        reply_err(req, err);

    if (path)
        free(path);
    pthread_rwlock_unlock(&f->tree_lock);
}