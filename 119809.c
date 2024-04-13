static int lo_fd(fuse_req_t req, fuse_ino_t ino)
{
    struct lo_inode *inode = lo_inode(req, ino);
    int fd;

    if (!inode) {
        return -1;
    }

    fd = inode->fd;
    lo_inode_put(lo_data(req), &inode);
    return fd;
}