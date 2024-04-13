static void fuse_lib_link(fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent,
                          const char *newname)
{
    struct fuse *f = req_fuse_prepare(req);
    struct fuse_entry_param e;
    char *oldpath;
    char *newpath;
    int err;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    oldpath = get_path(f, ino);
    if (oldpath != NULL) {
        newpath =  get_path_name(f, newparent, newname);
        if (newpath != NULL) {
            struct fuse_intr_data d;
            if (f->conf.debug)
                fprintf(stderr, "LINK %s\n", newpath);
            fuse_prepare_interrupt(f, req, &d);
            err = fuse_fs_link(f->fs, oldpath, newpath);
            if (!err)
                err = lookup_path(f, newparent, newname, newpath, &e, NULL);
            fuse_finish_interrupt(f, req, &d);
            free(newpath);
        }
        free(oldpath);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    reply_entry(req, &e, err);
}