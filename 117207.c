static void fuse_lib_rename(fuse_req_t req, fuse_ino_t olddir,
                            const char *oldname, fuse_ino_t newdir,
                            const char *newname)
{
    struct fuse *f = req_fuse_prepare(req);
    char *oldpath;
    char *newpath;
    int err;

    err = -ENOENT;
    pthread_rwlock_wrlock(&f->tree_lock);
    oldpath = get_path_name(f, olddir, oldname);
    if (oldpath != NULL) {
        newpath = get_path_name(f, newdir, newname);
        if (newpath != NULL) {
            struct fuse_intr_data d;
            if (f->conf.debug)
                fprintf(stderr, "RENAME %s -> %s\n", oldpath, newpath);
            err = 0;
            fuse_prepare_interrupt(f, req, &d);
            if (!f->conf.hard_remove && is_open(f, newdir, newname))
                err = hide_node(f, newpath, newdir, newname);
            if (!err) {
                err = fuse_fs_rename(f->fs, oldpath, newpath);
                if (!err)
                    err = rename_node(f, olddir, oldname, newdir, newname, 0);
            }
            fuse_finish_interrupt(f, req, &d);
            free(newpath);
        }
        free(oldpath);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    reply_err(req, err);
}