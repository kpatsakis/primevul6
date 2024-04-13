static void lo_flush(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    int res;
    (void)ino;
    struct lo_inode *inode;

    inode = lo_inode(req, ino);
    if (!inode) {
        fuse_reply_err(req, EBADF);
        return;
    }

    /* An fd is going away. Cleanup associated posix locks */
    pthread_mutex_lock(&inode->plock_mutex);
    g_hash_table_remove(inode->posix_locks, GUINT_TO_POINTER(fi->lock_owner));
    pthread_mutex_unlock(&inode->plock_mutex);

    res = close(dup(lo_fi_fd(req, fi)));
    lo_inode_put(lo_data(req), &inode);
    fuse_reply_err(req, res == -1 ? errno : 0);
}