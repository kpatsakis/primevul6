static struct fuse_dh *get_dirhandle(const struct fuse_file_info *llfi,
                                     struct fuse_file_info *fi)
{
    struct fuse_dh *dh = (struct fuse_dh *) (uintptr_t) llfi->fh;
    memset(fi, 0, sizeof(struct fuse_file_info));
    fi->fh = dh->fh;
    fi->fh_old = dh->fh;
    return dh;
}