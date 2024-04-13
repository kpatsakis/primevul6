static struct lo_inode *lo_inode(fuse_req_t req, fuse_ino_t ino)
{
    struct lo_data *lo = lo_data(req);
    struct lo_map_elem *elem;

    pthread_mutex_lock(&lo->mutex);
    elem = lo_map_get(&lo->ino_map, ino);
    if (elem) {
        g_atomic_int_inc(&elem->inode->refcount);
    }
    pthread_mutex_unlock(&lo->mutex);

    if (!elem) {
        return NULL;
    }

    return elem->inode;
}