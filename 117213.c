static void fuse_restore_intr_signal(int signum)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = SIG_DFL;
    sigaction(signum, &sa, NULL);
}