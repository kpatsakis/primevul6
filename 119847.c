static int lo_fi_fd(fuse_req_t req, struct fuse_file_info *fi)
{
    struct lo_data *lo = lo_data(req);
    struct lo_map_elem *elem;

    pthread_mutex_lock(&lo->mutex);
    elem = lo_map_get(&lo->fd_map, fi->fh);
    pthread_mutex_unlock(&lo->mutex);

    if (!elem) {
        return -1;
    }

    return elem->fd;
}