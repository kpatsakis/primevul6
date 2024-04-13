static void fuse_lib_fsync(fuse_req_t req, fuse_ino_t ino, int datasync,
                       struct fuse_file_info *fi)
{
    struct fuse *f = req_fuse_prepare(req);
    char *path;
    int err;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path != NULL) {
        struct fuse_intr_data d;
        if (f->conf.debug)
            fprintf(stderr, "FSYNC[%llu]\n", (unsigned long long) fi->fh);
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_fsync(f->fs, path, datasync, fi);
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    reply_err(req, err);
}