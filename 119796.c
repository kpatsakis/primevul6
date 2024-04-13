static void lo_forget_one(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup)
{
    struct lo_data *lo = lo_data(req);
    struct lo_inode *inode;

    inode = lo_inode(req, ino);
    if (!inode) {
        return;
    }

    fuse_log(FUSE_LOG_DEBUG, "  forget %lli %lli -%lli\n",
             (unsigned long long)ino, (unsigned long long)inode->nlookup,
             (unsigned long long)nlookup);

    unref_inode_lolocked(lo, inode, nlookup);
    lo_inode_put(lo, &inode);
}