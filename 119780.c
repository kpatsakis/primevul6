static int lo_do_lookup(fuse_req_t req, fuse_ino_t parent, const char *name,
                        struct fuse_entry_param *e)
{
    int newfd;
    int res;
    int saverr;
    struct lo_data *lo = lo_data(req);
    struct lo_inode *inode = NULL;
    struct lo_inode *dir = lo_inode(req, parent);

    /*
     * name_to_handle_at() and open_by_handle_at() can reach here with fuse
     * mount point in guest, but we don't have its inode info in the
     * ino_map.
     */
    if (!dir) {
        return ENOENT;
    }

    memset(e, 0, sizeof(*e));
    e->attr_timeout = lo->timeout;
    e->entry_timeout = lo->timeout;

    /* Do not allow escaping root directory */
    if (dir == &lo->root && strcmp(name, "..") == 0) {
        name = ".";
    }

    newfd = openat(dir->fd, name, O_PATH | O_NOFOLLOW);
    if (newfd == -1) {
        goto out_err;
    }

    res = fstatat(newfd, "", &e->attr, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
    if (res == -1) {
        goto out_err;
    }

    inode = lo_find(lo, &e->attr);
    if (inode) {
        close(newfd);
    } else {
        inode = calloc(1, sizeof(struct lo_inode));
        if (!inode) {
            goto out_err;
        }

        /* cache only filetype */
        inode->filetype = (e->attr.st_mode & S_IFMT);

        /*
         * One for the caller and one for nlookup (released in
         * unref_inode_lolocked())
         */
        g_atomic_int_set(&inode->refcount, 2);

        inode->nlookup = 1;
        inode->fd = newfd;
        inode->key.ino = e->attr.st_ino;
        inode->key.dev = e->attr.st_dev;
        pthread_mutex_init(&inode->plock_mutex, NULL);
        inode->posix_locks = g_hash_table_new_full(
            g_direct_hash, g_direct_equal, NULL, posix_locks_value_destroy);

        pthread_mutex_lock(&lo->mutex);
        inode->fuse_ino = lo_add_inode_mapping(req, inode);
        g_hash_table_insert(lo->inodes, &inode->key, inode);
        pthread_mutex_unlock(&lo->mutex);
    }
    e->ino = inode->fuse_ino;
    lo_inode_put(lo, &inode);
    lo_inode_put(lo, &dir);

    fuse_log(FUSE_LOG_DEBUG, "  %lli/%s -> %lli\n", (unsigned long long)parent,
             name, (unsigned long long)e->ino);

    return 0;

out_err:
    saverr = errno;
    if (newfd != -1) {
        close(newfd);
    }
    lo_inode_put(lo, &inode);
    lo_inode_put(lo, &dir);
    return saverr;
}