static void fuse_lib_write(fuse_req_t req, fuse_ino_t ino, const char *buf,
                       size_t size, off_t off, struct fuse_file_info *fi)
{
    struct fuse *f = req_fuse_prepare(req);
    char *path;
    int res;

    res = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path != NULL) {
        struct fuse_intr_data d;
        if (f->conf.debug)
            fprintf(stderr, "WRITE%s[%llu] %lu bytes to %llu\n",
                    fi->writepage ? "PAGE" : "", (unsigned long long) fi->fh,
                    (unsigned long) size, (unsigned long long) off);

        fuse_prepare_interrupt(f, req, &d);
        res = fuse_fs_write(f->fs, path, buf, size, off, fi);
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);

    if (res >= 0) {
        if (f->conf.debug)
            fprintf(stderr, "   WRITE%s[%llu] %u bytes\n",
                    fi->writepage ? "PAGE" : "", (unsigned long long) fi->fh,
                    res);
        if ((size_t) res > size)
            fprintf(stderr, "fuse: wrote too many bytes");
        fuse_reply_write(req, res);
    } else
        reply_err(req, res);
}