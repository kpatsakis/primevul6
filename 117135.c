static void fuse_lib_mkdir(fuse_req_t req, fuse_ino_t parent, const char *name,
                           mode_t mode)
{
    struct fuse *f = req_fuse_prepare(req);
    struct fuse_entry_param e;
    char *path;
    int err;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path_name(f, parent, name);
    if (path != NULL) {
        struct fuse_intr_data d;
        if (f->conf.debug)
            fprintf(stderr, "MKDIR %s\n", path);
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_mkdir(f->fs, path, mode);
        if (!err)
            err = lookup_path(f, parent, name, path, &e, NULL);
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    reply_entry(req, &e, err);
}