static void unref_inode_lolocked(struct lo_data *lo, struct lo_inode *inode,
                                 uint64_t n)
{
    if (!inode) {
        return;
    }

    pthread_mutex_lock(&lo->mutex);
    unref_inode(lo, inode, n);
    pthread_mutex_unlock(&lo->mutex);
}