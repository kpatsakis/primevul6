static void fuse_lib_flush(fuse_req_t req, fuse_ino_t ino,
                       struct fuse_file_info *fi)
{
    struct fuse *f = req_fuse_prepare(req);
    char *path;
    int err;

    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path && f->conf.debug)
        fprintf(stderr, "FLUSH[%llu]\n", (unsigned long long) fi->fh);
    err = fuse_flush_common(f, req, ino, path, fi);
    free(path);
    pthread_rwlock_unlock(&f->tree_lock);
    reply_err(req, err);
}