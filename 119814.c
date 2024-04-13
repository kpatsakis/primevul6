static void setup_mounts(const char *source)
{
    int oldroot;
    int newroot;

    if (mount(source, source, NULL, MS_BIND | MS_REC, NULL) < 0) {
        fuse_log(FUSE_LOG_ERR, "mount(%s, %s, MS_BIND): %m\n", source, source);
        exit(1);
    }

    /* This magic is based on lxc's lxc_pivot_root() */
    oldroot = open("/", O_DIRECTORY | O_RDONLY | O_CLOEXEC);
    if (oldroot < 0) {
        fuse_log(FUSE_LOG_ERR, "open(/): %m\n");
        exit(1);
    }

    newroot = open(source, O_DIRECTORY | O_RDONLY | O_CLOEXEC);
    if (newroot < 0) {
        fuse_log(FUSE_LOG_ERR, "open(%s): %m\n", source);
        exit(1);
    }

    if (fchdir(newroot) < 0) {
        fuse_log(FUSE_LOG_ERR, "fchdir(newroot): %m\n");
        exit(1);
    }

    if (syscall(__NR_pivot_root, ".", ".") < 0) {
        fuse_log(FUSE_LOG_ERR, "pivot_root(., .): %m\n");
        exit(1);
    }

    if (fchdir(oldroot) < 0) {
        fuse_log(FUSE_LOG_ERR, "fchdir(oldroot): %m\n");
        exit(1);
    }

    if (mount("", ".", "", MS_SLAVE | MS_REC, NULL) < 0) {
        fuse_log(FUSE_LOG_ERR, "mount(., MS_SLAVE | MS_REC): %m\n");
        exit(1);
    }

    if (umount2(".", MNT_DETACH) < 0) {
        fuse_log(FUSE_LOG_ERR, "umount2(., MNT_DETACH): %m\n");
        exit(1);
    }

    if (fchdir(newroot) < 0) {
        fuse_log(FUSE_LOG_ERR, "fchdir(newroot): %m\n");
        exit(1);
    }

    close(newroot);
    close(oldroot);
}