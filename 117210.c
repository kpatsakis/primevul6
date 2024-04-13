static void fuse_lib_releasedir(fuse_req_t req, fuse_ino_t ino,
                            struct fuse_file_info *llfi)
{
    struct fuse *f = req_fuse_prepare(req);
    struct fuse_intr_data d;
    struct fuse_file_info fi;
    struct fuse_dh *dh = get_dirhandle(llfi, &fi);
    char *path;

    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    fuse_prepare_interrupt(f, req, &d);
    fuse_fs_releasedir(f->fs, path ? path : "-", &fi);
    fuse_finish_interrupt(f, req, &d);
    if (path)
        free(path);
    pthread_rwlock_unlock(&f->tree_lock);
    pthread_mutex_lock(&dh->lock);
    pthread_mutex_unlock(&dh->lock);
    pthread_mutex_destroy(&dh->lock);
    free(dh->contents);
    free(dh);
    reply_err(req, 0);
}