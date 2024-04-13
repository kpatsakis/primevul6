static struct lo_inode_plock *lookup_create_plock_ctx(struct lo_data *lo,
                                                      struct lo_inode *inode,
                                                      uint64_t lock_owner,
                                                      pid_t pid, int *err)
{
    struct lo_inode_plock *plock;
    char procname[64];
    int fd;

    plock =
        g_hash_table_lookup(inode->posix_locks, GUINT_TO_POINTER(lock_owner));

    if (plock) {
        return plock;
    }

    plock = malloc(sizeof(struct lo_inode_plock));
    if (!plock) {
        *err = ENOMEM;
        return NULL;
    }

    /* Open another instance of file which can be used for ofd locks. */
    sprintf(procname, "%i", inode->fd);

    /* TODO: What if file is not writable? */
    fd = openat(lo->proc_self_fd, procname, O_RDWR);
    if (fd == -1) {
        *err = errno;
        free(plock);
        return NULL;
    }

    plock->lock_owner = lock_owner;
    plock->fd = fd;
    g_hash_table_insert(inode->posix_locks, GUINT_TO_POINTER(plock->lock_owner),
                        plock);
    return plock;
}