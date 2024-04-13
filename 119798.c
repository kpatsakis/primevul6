static void lo_destroy(void *userdata)
{
    struct lo_data *lo = (struct lo_data *)userdata;

    pthread_mutex_lock(&lo->mutex);
    while (true) {
        GHashTableIter iter;
        gpointer key, value;

        g_hash_table_iter_init(&iter, lo->inodes);
        if (!g_hash_table_iter_next(&iter, &key, &value)) {
            break;
        }

        struct lo_inode *inode = value;
        unref_inode(lo, inode, inode->nlookup);
    }
    pthread_mutex_unlock(&lo->mutex);
}