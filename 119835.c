static struct lo_inode *lookup_name(fuse_req_t req, fuse_ino_t parent,
                                    const char *name)
{
    int res;
    struct stat attr;

    res = fstatat(lo_fd(req, parent), name, &attr,
                  AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
    if (res == -1) {
        return NULL;
    }

    return lo_find(lo_data(req), &attr);
}