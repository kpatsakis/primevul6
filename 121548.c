uint64_t get_time_us(bool force) {
#ifdef _WIN32
    return std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
#else
    if (force || !g_use_seccomp) {
        struct timeval val = {0,0};
        gettimeofday(&val,NULL);
        uint64_t retval = val.tv_sec;
        retval *= 1000000;
        retval += val.tv_usec;
        return retval;
    }
#endif
    return 0;
}