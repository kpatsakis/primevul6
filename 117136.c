static void fuse_lib_bmap(fuse_req_t req, fuse_ino_t ino, size_t blocksize,
                          uint64_t idx)
{
    struct fuse *f = req_fuse_prepare(req);
    struct fuse_intr_data d;
    char *path;
    int err;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path != NULL) {
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_bmap(f->fs, path, blocksize, &idx);
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    if (!err)
        fuse_reply_bmap(req, idx);
    else
        reply_err(req, err);
}