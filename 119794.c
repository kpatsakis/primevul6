static void posix_locks_value_destroy(gpointer data)
{
    struct lo_inode_plock *plock = data;

    /*
     * We had used open() for locks and had only one fd. So
     * closing this fd should release all OFD locks.
     */
    close(plock->fd);
    free(plock);
}