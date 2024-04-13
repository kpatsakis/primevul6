static void lo_release(fuse_req_t req, fuse_ino_t ino,
                       struct fuse_file_info *fi)
{
    struct lo_data *lo = lo_data(req);
    struct lo_map_elem *elem;
    int fd = -1;

    (void)ino;

    pthread_mutex_lock(&lo->mutex);
    elem = lo_map_get(&lo->fd_map, fi->fh);
    if (elem) {
        fd = elem->fd;
        elem = NULL;
        lo_map_remove(&lo->fd_map, fi->fh);
    }
    pthread_mutex_unlock(&lo->mutex);

    close(fd);
    fuse_reply_err(req, 0);
}