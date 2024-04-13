void test_syscall_injection(std::atomic<int>*value) {
#ifndef _WIN32
    char buf[128 + 1];
    buf[sizeof(buf) - 1] = 0;
    value->store(-1);
    char * ret = getcwd(buf, sizeof(buf) - 1);
    value->store(ret ? 1 : 2);
#endif
}