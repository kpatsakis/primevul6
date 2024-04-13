void timing_operation_first_byte( char operation ) {
#ifndef _WIN32
    if (g_use_seccomp) {
        return;
    }
    dev_assert(current_operation == operation);
#ifdef _WIN32
    if (current_operation_first_byte == 0) {
        current_operation_first_byte = clock();
    }
#else
    if (current_operation_first_byte.tv_sec == 0 &&
        current_operation_first_byte.tv_usec == 0) {
        gettimeofday(&current_operation_first_byte, NULL);
    }
#endif
#endif
}