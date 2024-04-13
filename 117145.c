static void fuse_lib_opendir(fuse_req_t req, fuse_ino_t ino,
                       struct fuse_file_info *llfi)
{
    struct fuse *f = req_fuse_prepare(req);
    struct fuse_intr_data d;
    struct fuse_dh *dh;
    struct fuse_file_info fi;
    char *path;
    int err;

    dh = (struct fuse_dh *) malloc(sizeof(struct fuse_dh));
    if (dh == NULL) {
        reply_err(req, -ENOMEM);
        return;
    }
    memset(dh, 0, sizeof(struct fuse_dh));
    dh->fuse = f;
    dh->contents = NULL;
    dh->len = 0;
    dh->filled = 0;
    dh->nodeid = ino;
    fuse_mutex_init(&dh->lock);

    llfi->fh = (uintptr_t) dh;

    memset(&fi, 0, sizeof(fi));
    fi.flags = llfi->flags;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path != NULL) {
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_opendir(f->fs, path, &fi);
        fuse_finish_interrupt(f, req, &d);
        dh->fh = fi.fh;
    }
    if (!err) {
        if (fuse_reply_open(req, llfi) == -ENOENT) {
            /* The opendir syscall was interrupted, so it must be cancelled */
            fuse_prepare_interrupt(f, req, &d);
            fuse_fs_releasedir(f->fs, path, &fi);
            fuse_finish_interrupt(f, req, &d);
            pthread_mutex_destroy(&dh->lock);
            free(dh);
        }
    } else {
        reply_err(req, err);
        pthread_mutex_destroy(&dh->lock);
        free(dh);
    }
    free(path);
    pthread_rwlock_unlock(&f->tree_lock);
}