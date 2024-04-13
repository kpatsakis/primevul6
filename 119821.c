static void setup_root(struct lo_data *lo, struct lo_inode *root)
{
    int fd, res;
    struct stat stat;

    fd = open("/", O_PATH);
    if (fd == -1) {
        fuse_log(FUSE_LOG_ERR, "open(%s, O_PATH): %m\n", lo->source);
        exit(1);
    }

    res = fstatat(fd, "", &stat, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
    if (res == -1) {
        fuse_log(FUSE_LOG_ERR, "fstatat(%s): %m\n", lo->source);
        exit(1);
    }

    root->filetype = S_IFDIR;
    root->fd = fd;
    root->key.ino = stat.st_ino;
    root->key.dev = stat.st_dev;
    root->nlookup = 2;
    g_atomic_int_set(&root->refcount, 2);
}