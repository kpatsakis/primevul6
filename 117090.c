static int fuse_init_intr_signal(int signum, int *installed)
{
    struct sigaction old_sa;

    if (sigaction(signum, NULL, &old_sa) == -1) {
        perror("fuse: cannot get old signal handler");
        return -1;
    }

    if (old_sa.sa_handler == SIG_DFL) {
        struct sigaction sa;

        memset(&sa, 0, sizeof(struct sigaction));
        sa.sa_handler = fuse_intr_sighandler;
        sigemptyset(&sa.sa_mask);

        if (sigaction(signum, &sa, NULL) == -1) {
            perror("fuse: cannot set interrupt signal handler");
            return -1;
        }
        *installed = 1;
    }
    return 0;
}