static void fuse_lib_getattr(fuse_req_t req, fuse_ino_t ino,
                             struct fuse_file_info *fi)
{
    struct fuse *f = req_fuse_prepare(req);
    struct stat buf;
    char *path;
    int err;

    (void) fi;
    memset(&buf, 0, sizeof(buf));

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path != NULL) {
        struct fuse_intr_data d;
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_getattr(f->fs, path, &buf);
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    if (!err) {
#ifdef __SOLARIS__
        if (f->conf.auto_cache) {
            pthread_mutex_lock(&f->lock);
            update_stat(get_node(f, ino), &buf);
            pthread_mutex_unlock(&f->lock);
        }
#endif /* __SOLARIS__ */
        set_stat(f, ino, &buf);
        fuse_reply_attr(req, &buf, f->conf.attr_timeout);
    } else
        reply_err(req, err);
}