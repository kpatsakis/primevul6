static struct lo_inode *lo_find(struct lo_data *lo, struct stat *st)
{
    struct lo_inode *p;
    struct lo_key key = {
        .ino = st->st_ino,
        .dev = st->st_dev,
    };

    pthread_mutex_lock(&lo->mutex);
    p = g_hash_table_lookup(lo->inodes, &key);
    if (p) {
        assert(p->nlookup > 0);
        p->nlookup++;
        g_atomic_int_inc(&p->refcount);
    }
    pthread_mutex_unlock(&lo->mutex);

    return p;
}