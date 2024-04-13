static void curr_time(struct timespec *now)
{
    static clockid_t clockid = CLOCK_MONOTONIC;
    int res = clock_gettime(clockid, now);
    if (res == -1 && errno == EINVAL) {
        clockid = CLOCK_REALTIME;
        res = clock_gettime(clockid, now);
    }
    if (res == -1) {
        perror("fuse: clock_gettime");
        abort();
    }
}