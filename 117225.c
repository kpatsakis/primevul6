static void fuse_lib_readlink(fuse_req_t req, fuse_ino_t ino)
{
    struct fuse *f = req_fuse_prepare(req);
    char linkname[PATH_MAX + 1];
    char *path;
    int err;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path != NULL) {
        struct fuse_intr_data d;
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_readlink(f->fs, path, linkname, sizeof(linkname));
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    if (!err) {
        linkname[PATH_MAX] = '\0';
        fuse_reply_readlink(req, linkname);
    } else
        reply_err(req, err);
}