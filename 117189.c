static void fuse_lib_mknod(fuse_req_t req, fuse_ino_t parent, const char *name,
                           mode_t mode, dev_t rdev)
{
    struct fuse *f = req_fuse_prepare(req);
    struct fuse_entry_param e;
    char *path;
    int err;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path_name(f, parent, name);
    if (path) {
        struct fuse_intr_data d;
        if (f->conf.debug)
            fprintf(stderr, "MKNOD %s\n", path);
        fuse_prepare_interrupt(f, req, &d);
        err = -ENOSYS;
        if (S_ISREG(mode)) {
            struct fuse_file_info fi;

            memset(&fi, 0, sizeof(fi));
            fi.flags = O_CREAT | O_EXCL | O_WRONLY;
            err = fuse_fs_create(f->fs, path, mode, &fi);
            if (!err) {
                err = lookup_path(f, parent, name, path, &e, &fi);
                fuse_fs_release(f->fs, path, &fi);
            }
        }
        if (err == -ENOSYS) {
            err = fuse_fs_mknod(f->fs, path, mode, rdev);
            if (!err)
                err = lookup_path(f, parent, name, path, &e, NULL);
        }
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    reply_entry(req, &e, err);
}