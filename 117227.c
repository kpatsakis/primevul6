static double diff_timespec(const struct timespec *t1,
                            const struct timespec *t2)
{
    return (t1->tv_sec - t2->tv_sec) + 
        ((double) t1->tv_nsec - (double) t2->tv_nsec) / 1000000000.0;
}