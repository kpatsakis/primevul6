static void fuse_lib_unlink(fuse_req_t req, fuse_ino_t parent,
                            const char *name)
{
    struct fuse *f = req_fuse_prepare(req);
    char *path;
    int err;

    err = -ENOENT;
    pthread_rwlock_wrlock(&f->tree_lock);
    path = get_path_name(f, parent, name);
    if (path != NULL) {
        struct fuse_intr_data d;
        if (f->conf.debug)
            fprintf(stderr, "UNLINK %s\n", path);
        fuse_prepare_interrupt(f, req, &d);
        if (!f->conf.hard_remove && is_open(f, parent, name))
            err = hide_node(f, path, parent, name);
        else {
            err = fuse_fs_unlink(f->fs, path);
            if (!err)
                remove_node(f, parent, name);
        }
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    reply_err(req, err);
}