static void fuse_lib_release(fuse_req_t req, fuse_ino_t ino,
                             struct fuse_file_info *fi)
{
    struct fuse *f = req_fuse_prepare(req);
    struct fuse_intr_data d;
    char *path;
    int err = 0;

    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (f->conf.debug)
        fprintf(stderr, "RELEASE%s[%llu] flags: 0x%x\n",
                fi->flush ? "+FLUSH" : "",
                (unsigned long long) fi->fh, fi->flags);

    if (fi->flush) {
        err = fuse_flush_common(f, req, ino, path, fi);
        if (err == -ENOSYS)
            err = 0;
    }

    fuse_prepare_interrupt(f, req, &d);
    fuse_do_release(f, ino, path, fi);
    fuse_finish_interrupt(f, req, &d);
    free(path);
    pthread_rwlock_unlock(&f->tree_lock);

    reply_err(req, err);
}