bool recode_baseline_jpeg_wrapper() {
    bool retval = recode_baseline_jpeg(str_out, max_file_size);
    if (!retval) {
        errorlevel.store(2);
        return retval;
    }
    // get filesize
    jpgfilesize = str_out->getsize();
    if (ujg_base_in) {
        ujgfilesize = ujg_base_in->getsize();
    } else {
        ujgfilesize = 4096 * 1024;
    }
#ifndef _WIN32
    if (!g_use_seccomp) {
        clock_t final = clock();
        struct timeval fin = {0,0};
        gettimeofday(&fin,NULL);
        double begin = current_operation_begin.tv_sec + (double)current_operation_begin.tv_usec / 1000000.;
        double end = fin.tv_sec + (double)fin.tv_usec / 1000000.;
        double first_byte = current_operation_first_byte.tv_sec + (double)current_operation_first_byte.tv_usec / 1000000.;
        double begin_to_end = end - begin;
        double begin_to_first_byte = begin_to_end;
        if (current_operation_first_byte.tv_sec != 0) { // if we were successful
            begin_to_first_byte = first_byte - begin;
        }

        fprintf(stderr, "TIMING (new method): %f to first byte %f total\n",
                begin_to_first_byte,
                begin_to_end);
        (void)final;
        fprintf(stderr, "Read took: %f\n",
                (read_done - overall_start)/(double)CLOCKS_PER_SEC);
    }
#endif
    // store last scan & restart positions
    if ( !rstp.empty() )
        rstp.at(rstc) = hufs;


    return retval;
}