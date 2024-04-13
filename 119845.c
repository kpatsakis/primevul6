static void setup_wait_parent_capabilities(void)
{
    capng_setpid(syscall(SYS_gettid));
    capng_clear(CAPNG_SELECT_BOTH);
    capng_apply(CAPNG_SELECT_BOTH);
}