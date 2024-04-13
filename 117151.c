static void fuse_lib_setxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
                              const char *value, size_t size, int flags)
{
    struct fuse *f = req_fuse_prepare(req);
    char *path;
    int err;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path != NULL) {
        struct fuse_intr_data d;
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_setxattr(f->fs, path, name, value, size, flags);
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    reply_err(req, err);
}