void timing_operation_complete( char operation ) {
#ifndef _WIN32
    if (g_use_seccomp) {
        return;
    }
    dev_assert(current_operation == operation);
#ifdef _WIN32
    current_operation_end = clock();
    if (timing_log) {
        double begin_to_end = (current_operation_end - current_operation_begin) / (double)CLOCKS_PER_SEC;
        double begin_to_first_byte = begin_to_end;
        if (current_operation_first_byte != 0) { // if we were successful
            begin_to_first_byte = (current_operation_first_byte - current_operation_begin) / (double)CLOCKS_PER_SEC;
        }
        fprintf(timing_log, "%c %f %f\n", current_operation, begin_to_first_byte, begin_to_end);
        fflush(timing_log);
    }
    current_operation_end = 0;
    current_operation_begin = 0;
    current_operation_first_byte = 0;
#else
    gettimeofday(&current_operation_end, NULL);
    if (timing_log) {
        double begin = current_operation_begin.tv_sec + (double)current_operation_begin.tv_usec / 1000000.;
        double end = current_operation_end.tv_sec + (double)current_operation_end.tv_usec / 1000000.;
        double first_byte = current_operation_first_byte.tv_sec + (double)current_operation_first_byte.tv_usec / 1000000.;
        double begin_to_end = end - begin;
        double begin_to_first_byte = begin_to_end;
        if (current_operation_first_byte.tv_sec != 0) { // if we were successful
            begin_to_first_byte = first_byte - begin;
        }
        fprintf(timing_log, "%c %f %f\n", current_operation, begin_to_first_byte, begin_to_end);
        fflush(timing_log);
    }
    memset(&current_operation_end, 0, sizeof(current_operation_end));
    memset(&current_operation_begin, 0, sizeof(current_operation_begin));
    memset(&current_operation_first_byte, 0, sizeof(current_operation_first_byte));
#endif
#endif
}