static void update_open_flags(int writeback, int allow_direct_io,
                              struct fuse_file_info *fi)
{
    /*
     * With writeback cache, kernel may send read requests even
     * when userspace opened write-only
     */
    if (writeback && (fi->flags & O_ACCMODE) == O_WRONLY) {
        fi->flags &= ~O_ACCMODE;
        fi->flags |= O_RDWR;
    }

    /*
     * With writeback cache, O_APPEND is handled by the kernel.
     * This breaks atomicity (since the file may change in the
     * underlying filesystem, so that the kernel's idea of the
     * end of the file isn't accurate anymore). In this example,
     * we just accept that. A more rigorous filesystem may want
     * to return an error here
     */
    if (writeback && (fi->flags & O_APPEND)) {
        fi->flags &= ~O_APPEND;
    }

    /*
     * O_DIRECT in guest should not necessarily mean bypassing page
     * cache on host as well. Therefore, we discard it by default
     * ('-o no_allow_direct_io'). If somebody needs that behavior,
     * the '-o allow_direct_io' option should be set.
     */
    if (!allow_direct_io) {
        fi->flags &= ~O_DIRECT;
    }
}