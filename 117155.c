static char *get_path(struct fuse *f, fuse_ino_t nodeid)
{
    return get_path_name(f, nodeid, NULL);
}