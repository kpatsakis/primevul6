static void fuse_lib_statfs(fuse_req_t req, fuse_ino_t ino)
{
    struct fuse *f = req_fuse_prepare(req);
    struct statvfs buf;
    char *path;
    int err;

    memset(&buf, 0, sizeof(buf));
    pthread_rwlock_rdlock(&f->tree_lock);
    if (!ino) {
        err = -ENOMEM;
        path = strdup("/");
    } else {
        err = -ENOENT;
        path = get_path(f, ino);
    }
    if (path) {
        struct fuse_intr_data d;
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_statfs(f->fs, path, &buf);
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);

    if (!err)
        fuse_reply_statfs(req, &buf);
    else
        reply_err(req, err);
}