static int mtime_eq(const struct stat *stbuf, const struct timespec *ts)
{
    return stbuf->st_mtime == ts->tv_sec && ST_MTIM_NSEC(stbuf) == ts->tv_nsec;
}