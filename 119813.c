static void lo_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    int fd;
    ssize_t fh;
    char buf[64];
    struct lo_data *lo = lo_data(req);

    fuse_log(FUSE_LOG_DEBUG, "lo_open(ino=%" PRIu64 ", flags=%d)\n", ino,
             fi->flags);

    update_open_flags(lo->writeback, lo->allow_direct_io, fi);

    sprintf(buf, "%i", lo_fd(req, ino));
    fd = openat(lo->proc_self_fd, buf, fi->flags & ~O_NOFOLLOW);
    if (fd == -1) {
        return (void)fuse_reply_err(req, errno);
    }

    pthread_mutex_lock(&lo->mutex);
    fh = lo_add_fd_mapping(req, fd);
    pthread_mutex_unlock(&lo->mutex);
    if (fh == -1) {
        close(fd);
        fuse_reply_err(req, ENOMEM);
        return;
    }

    fi->fh = fh;
    if (lo->cache == CACHE_NONE) {
        fi->direct_io = 1;
    } else if (lo->cache == CACHE_ALWAYS) {
        fi->keep_cache = 1;
    }
    fuse_reply_open(req, fi);
}