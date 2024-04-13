static struct lo_dirp *lo_dirp(fuse_req_t req, struct fuse_file_info *fi)
{
    struct lo_data *lo = lo_data(req);
    struct lo_map_elem *elem;

    pthread_mutex_lock(&lo->mutex);
    elem = lo_map_get(&lo->dirp_map, fi->fh);
    if (elem) {
        g_atomic_int_inc(&elem->dirp->refcount);
    }
    pthread_mutex_unlock(&lo->mutex);
    if (!elem) {
        return NULL;
    }

    return elem->dirp;
}