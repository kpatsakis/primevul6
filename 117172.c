static int readdir_fill(struct fuse *f, fuse_req_t req, fuse_ino_t ino,
                        size_t size, off_t off, struct fuse_dh *dh,
                        struct fuse_file_info *fi)
{
    int err = -ENOENT;
    char *path;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path != NULL) {
        struct fuse_intr_data d;

        dh->len = 0;
        dh->error = 0;
        dh->needlen = size;
        dh->filled = 1;
        dh->req = req;
        fuse_prepare_interrupt(f, req, &d);
        err = fuse_fs_readdir(f->fs, path, dh, fill_dir, off, fi);
        fuse_finish_interrupt(f, req, &d);
        dh->req = NULL;
        if (!err)
            err = dh->error;
        if (err)
            dh->filled = 0;
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    return err;
}