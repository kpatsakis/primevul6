static void lo_inode_put(struct lo_data *lo, struct lo_inode **inodep)
{
    struct lo_inode *inode = *inodep;

    if (!inode) {
        return;
    }

    *inodep = NULL;

    if (g_atomic_int_dec_and_test(&inode->refcount)) {
        close(inode->fd);
        free(inode);
    }
}