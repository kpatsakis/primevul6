static void lo_init(void *userdata, struct fuse_conn_info *conn)
{
    struct lo_data *lo = (struct lo_data *)userdata;

    if (conn->capable & FUSE_CAP_EXPORT_SUPPORT) {
        conn->want |= FUSE_CAP_EXPORT_SUPPORT;
    }

    if (lo->writeback && conn->capable & FUSE_CAP_WRITEBACK_CACHE) {
        fuse_log(FUSE_LOG_DEBUG, "lo_init: activating writeback\n");
        conn->want |= FUSE_CAP_WRITEBACK_CACHE;
    }
    if (conn->capable & FUSE_CAP_FLOCK_LOCKS) {
        if (lo->flock) {
            fuse_log(FUSE_LOG_DEBUG, "lo_init: activating flock locks\n");
            conn->want |= FUSE_CAP_FLOCK_LOCKS;
        } else {
            fuse_log(FUSE_LOG_DEBUG, "lo_init: disabling flock locks\n");
            conn->want &= ~FUSE_CAP_FLOCK_LOCKS;
        }
    }

    if (conn->capable & FUSE_CAP_POSIX_LOCKS) {
        if (lo->posix_lock) {
            fuse_log(FUSE_LOG_DEBUG, "lo_init: activating posix locks\n");
            conn->want |= FUSE_CAP_POSIX_LOCKS;
        } else {
            fuse_log(FUSE_LOG_DEBUG, "lo_init: disabling posix locks\n");
            conn->want &= ~FUSE_CAP_POSIX_LOCKS;
        }
    }

    if ((lo->cache == CACHE_NONE && !lo->readdirplus_set) ||
        lo->readdirplus_clear) {
        fuse_log(FUSE_LOG_DEBUG, "lo_init: disabling readdirplus\n");
        conn->want &= ~FUSE_CAP_READDIRPLUS;
    }
}