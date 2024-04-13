static void lo_setxattr(fuse_req_t req, fuse_ino_t ino, const char *name,
                        const char *value, size_t size, int flags)
{
    char procname[64];
    struct lo_data *lo = lo_data(req);
    struct lo_inode *inode;
    ssize_t ret;
    int saverr;
    int fd = -1;

    inode = lo_inode(req, ino);
    if (!inode) {
        fuse_reply_err(req, EBADF);
        return;
    }

    saverr = ENOSYS;
    if (!lo_data(req)->xattr) {
        goto out;
    }

    fuse_log(FUSE_LOG_DEBUG, "lo_setxattr(ino=%" PRIu64
             ", name=%s value=%s size=%zd)\n", ino, name, value, size);

    sprintf(procname, "%i", inode->fd);
    if (S_ISREG(inode->filetype) || S_ISDIR(inode->filetype)) {
        fd = openat(lo->proc_self_fd, procname, O_RDONLY);
        if (fd < 0) {
            saverr = errno;
            goto out;
        }
        ret = fsetxattr(fd, name, value, size, flags);
    } else {
        /* fchdir should not fail here */
        assert(fchdir(lo->proc_self_fd) == 0);
        ret = setxattr(procname, name, value, size, flags);
        assert(fchdir(lo->root.fd) == 0);
    }

    saverr = ret == -1 ? errno : 0;

out:
    if (fd >= 0) {
        close(fd);
    }

    lo_inode_put(lo, &inode);
    fuse_reply_err(req, saverr);
}