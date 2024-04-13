static void lo_listxattr(fuse_req_t req, fuse_ino_t ino, size_t size)
{
    struct lo_data *lo = lo_data(req);
    char *value = NULL;
    char procname[64];
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

    fuse_log(FUSE_LOG_DEBUG, "lo_listxattr(ino=%" PRIu64 ", size=%zd)\n", ino,
             size);

    if (size) {
        value = malloc(size);
        if (!value) {
            goto out_err;
        }
    }

    sprintf(procname, "%i", inode->fd);
    if (S_ISREG(inode->filetype) || S_ISDIR(inode->filetype)) {
        fd = openat(lo->proc_self_fd, procname, O_RDONLY);
        if (fd < 0) {
            goto out_err;
        }
        ret = flistxattr(fd, value, size);
    } else {
        /* fchdir should not fail here */
        assert(fchdir(lo->proc_self_fd) == 0);
        ret = listxattr(procname, value, size);
        assert(fchdir(lo->root.fd) == 0);
    }

    if (ret == -1) {
        goto out_err;
    }
    if (size) {
        saverr = 0;
        if (ret == 0) {
            goto out;
        }
        fuse_reply_buf(req, value, ret);
    } else {
        fuse_reply_xattr(req, ret);
    }
out_free:
    free(value);

    if (fd >= 0) {
        close(fd);
    }

    lo_inode_put(lo, &inode);
    return;

out_err:
    saverr = errno;
out:
    fuse_reply_err(req, saverr);
    goto out_free;
}