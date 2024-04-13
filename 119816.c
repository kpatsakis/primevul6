static void lo_releasedir(fuse_req_t req, fuse_ino_t ino,
                          struct fuse_file_info *fi)
{
    struct lo_data *lo = lo_data(req);
    struct lo_map_elem *elem;
    struct lo_dirp *d;

    (void)ino;

    pthread_mutex_lock(&lo->mutex);
    elem = lo_map_get(&lo->dirp_map, fi->fh);
    if (!elem) {
        pthread_mutex_unlock(&lo->mutex);
        fuse_reply_err(req, EBADF);
        return;
    }

    d = elem->dirp;
    lo_map_remove(&lo->dirp_map, fi->fh);
    pthread_mutex_unlock(&lo->mutex);

    lo_dirp_put(&d); /* paired with lo_opendir() */

    fuse_reply_err(req, 0);
}