static void setup_nofile_rlimit(unsigned long rlimit_nofile)
{
    struct rlimit rlim = {
        .rlim_cur = rlimit_nofile,
        .rlim_max = rlimit_nofile,
    };

    if (rlimit_nofile == 0) {
        return; /* nothing to do */
    }

    if (setrlimit(RLIMIT_NOFILE, &rlim) < 0) {
        /* Ignore SELinux denials */
        if (errno == EPERM) {
            return;
        }

        fuse_log(FUSE_LOG_ERR, "setrlimit(RLIMIT_NOFILE): %m\n");
        exit(1);
    }
}