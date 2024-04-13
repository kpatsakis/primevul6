void timing_operation_start( char operation ) {
#ifndef _WIN32
    if (g_use_seccomp) {
        return;
    }
    current_operation = operation;
#ifdef _WIN32
    current_operation_begin = clock();
    current_operation_first_byte = 0;
    current_operation_end = 0;
#else
    gettimeofday(&current_operation_begin, NULL);
    memset(&current_operation_first_byte, 0, sizeof(current_operation_first_byte));
    memset(&current_operation_end, 0, sizeof(current_operation_end));
#endif
    fprintf(stderr,"START ACHIEVED %ld %ld\n",
            (long)current_operation_begin.tv_sec, (long)current_operation_begin.tv_usec );
#endif
}