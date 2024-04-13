static void fuse_lib_setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr,
                             int valid, struct fuse_file_info *fi)
{
    struct fuse *f = req_fuse_prepare(req);
    struct stat buf;
    char *path;
    int err;

    err = -ENOENT;
    pthread_rwlock_rdlock(&f->tree_lock);
    path = get_path(f, ino);
    if (path != NULL) {
        struct fuse_intr_data d;
        fuse_prepare_interrupt(f, req, &d);
        err = 0;
        if (!err && (valid & FUSE_SET_ATTR_MODE))
            err = fuse_fs_chmod(f->fs, path, attr->st_mode);
        if (!err && (valid & (FUSE_SET_ATTR_UID | FUSE_SET_ATTR_GID))) {
            uid_t uid = 
                (valid & FUSE_SET_ATTR_UID) ? attr->st_uid : (uid_t) -1;
            gid_t gid = 
                (valid & FUSE_SET_ATTR_GID) ? attr->st_gid : (gid_t) -1;
            err = fuse_fs_chown(f->fs, path, uid, gid);
        }
        if (!err && (valid & FUSE_SET_ATTR_SIZE)) {
            if (fi)
                err = fuse_fs_ftruncate(f->fs, path, attr->st_size, fi);
            else
                err = fuse_fs_truncate(f->fs, path, attr->st_size);
        }
#ifdef HAVE_UTIMENSAT
        if (!err &&
            (valid & (FUSE_SET_ATTR_ATIME | FUSE_SET_ATTR_MTIME))) {
            struct timespec tv[2];

            tv[0].tv_sec = 0;
            tv[1].tv_sec = 0;
            tv[0].tv_nsec = UTIME_OMIT;
            tv[1].tv_nsec = UTIME_OMIT;

            if (valid & FUSE_SET_ATTR_ATIME_NOW)
                tv[0].tv_nsec = UTIME_NOW;
            else if (valid & FUSE_SET_ATTR_ATIME)
                tv[0] = attr->st_atim;

            if (valid & FUSE_SET_ATTR_MTIME_NOW)
                tv[1].tv_nsec = UTIME_NOW;
            else if (valid & FUSE_SET_ATTR_MTIME)
                tv[1] = attr->st_mtim;

            err = fuse_fs_utimens(f->fs, path, tv);
        } else
#endif
        if (!err && (valid & (FUSE_SET_ATTR_ATIME | FUSE_SET_ATTR_MTIME)) ==
            (FUSE_SET_ATTR_ATIME | FUSE_SET_ATTR_MTIME)) {
            struct timespec tv[2];
            tv[0].tv_sec = attr->st_atime;
            tv[0].tv_nsec = ST_ATIM_NSEC(attr);
            tv[1].tv_sec = attr->st_mtime;
            tv[1].tv_nsec = ST_MTIM_NSEC(attr);
            err = fuse_fs_utimens(f->fs, path, tv);
        }
        if (!err)
            err = fuse_fs_getattr(f->fs,  path, &buf);
        fuse_finish_interrupt(f, req, &d);
        free(path);
    }
    pthread_rwlock_unlock(&f->tree_lock);
    if (!err) {
#ifdef __SOLARIS__
        if (f->conf.auto_cache) {
            pthread_mutex_lock(&f->lock);
            update_stat(get_node(f, ino), &buf);
            pthread_mutex_unlock(&f->lock);
        }
#endif /* __SOLARIS__ */
        set_stat(f, ino, &buf);
        fuse_reply_attr(req, &buf, f->conf.attr_timeout);
    } else
        reply_err(req, err);
}