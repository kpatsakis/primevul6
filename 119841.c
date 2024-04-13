static void lo_opendir(fuse_req_t req, fuse_ino_t ino,
                       struct fuse_file_info *fi)
{
    int error = ENOMEM;
    struct lo_data *lo = lo_data(req);
    struct lo_dirp *d;
    int fd;
    ssize_t fh;

    d = calloc(1, sizeof(struct lo_dirp));
    if (d == NULL) {
        goto out_err;
    }

    fd = openat(lo_fd(req, ino), ".", O_RDONLY);
    if (fd == -1) {
        goto out_errno;
    }

    d->dp = fdopendir(fd);
    if (d->dp == NULL) {
        goto out_errno;
    }

    d->offset = 0;
    d->entry = NULL;

    g_atomic_int_set(&d->refcount, 1); /* paired with lo_releasedir() */
    pthread_mutex_lock(&lo->mutex);
    fh = lo_add_dirp_mapping(req, d);
    pthread_mutex_unlock(&lo->mutex);
    if (fh == -1) {
        goto out_err;
    }

    fi->fh = fh;
    if (lo->cache == CACHE_ALWAYS) {
        fi->cache_readdir = 1;
    }
    fuse_reply_open(req, fi);
    return;

out_errno:
    error = errno;
out_err:
    if (d) {
        if (d->dp) {
            closedir(d->dp);
        } else if (fd != -1) {
            close(fd);
        }
        free(d);
    }
    fuse_reply_err(req, error);
}