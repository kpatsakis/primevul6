static int fill_dir(void *dh_, const char *name, const struct stat *statp,
                    off_t off)
{
    struct fuse_dh *dh = (struct fuse_dh *) dh_;
    struct stat stbuf;
    size_t newlen;

    if (statp)
        stbuf = *statp;
    else {
        memset(&stbuf, 0, sizeof(stbuf));
        stbuf.st_ino = FUSE_UNKNOWN_INO;
    }

    if (!dh->fuse->conf.use_ino) {
        stbuf.st_ino = FUSE_UNKNOWN_INO;
        if (dh->fuse->conf.readdir_ino) {
            struct node *node;
            pthread_mutex_lock(&dh->fuse->lock);
            node = lookup_node(dh->fuse, dh->nodeid, name);
            if (node)
                stbuf.st_ino  = (ino_t) node->nodeid;
            pthread_mutex_unlock(&dh->fuse->lock);
        }
    }

    if (off) {
        if (extend_contents(dh, dh->needlen) == -1)
            return 1;

        dh->filled = 0;
        newlen = dh->len + fuse_add_direntry(dh->req, dh->contents + dh->len,
                                             dh->needlen - dh->len, name,
                                             &stbuf, off);
        if (newlen > dh->needlen)
            return 1;
    } else {
        newlen = dh->len + fuse_add_direntry(dh->req, NULL, 0, name, NULL, 0);
        if (extend_contents(dh, newlen) == -1)
            return 1;

        fuse_add_direntry(dh->req, dh->contents + dh->len, dh->size - dh->len,
                          name, &stbuf, newlen);
    }
    dh->len = newlen;
    return 0;
}