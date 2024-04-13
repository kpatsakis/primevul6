int EMSCRIPTEN_KEEPALIVE main(void) {
    const int argc = fake_argc;
    const char **argv = fake_argv;
    g_argc = argc;
    g_argv = argv;
    TimingHarness::timing[0][TimingHarness::TS_MAIN]
        = TimingHarness::get_time_us(true);
    size_t thread_mem_limit = 128 * 1024 * 1024;
    size_t mem_limit = 1280 * 1024 * 1024 - thread_mem_limit * (MAX_NUM_THREADS - 1);
    bool needs_huge_pages = false;
    for (int i = 1; i < argc; ++i) {
        bool avx2upgrade = false;
        compute_thread_mem(argv[i],
                           &mem_limit,
                           &thread_mem_limit,
                           &needs_huge_pages,
                           &avx2upgrade);
    }

    // the system needs 33 megs of ram ontop of the uncompressed image buffer.
    // This adds a few extra megs just to keep things real
    UncompressedComponents::max_number_of_blocks = ( mem_limit / 4 ) * 3;
    if (mem_limit > 48 * 1024 * 1024) {
        UncompressedComponents::max_number_of_blocks = mem_limit - 36 * 1024 * 1024;
    }
    UncompressedComponents::max_number_of_blocks /= (sizeof(uint16_t) * 64);
    int n_threads = MAX_NUM_THREADS - 1;
    clock_t begin = 0, end = 1;

    int error_cnt = 0;
    int warn_cnt  = 0;

    int acc_jpgsize = 0;
    int acc_ujgsize = 0;

    int speed, bpms;
    float cr;

    errorlevel.store(0);

    // read options from command line
    int max_file_size = initialize_options( argc, argv );
    if (action != forkserve && action != socketserve) {
        // write program info to screen
        fprintf( msgout,  "%s v%i.0-%s\n",
                 appname, ujgversion, GIT_REVISION );
    }
    // check if user input is wrong, show help screen if it is
    if ((file_cnt == 0 && action != forkserve && action != socketserve)
        || ((!developer) && ((action != comp && action != forkserve && action != socketserve)))) {
        show_help();
        return -1;
    }


    // (re)set program has to be done first
    reset_buffers();

    // process file(s) - this is the main function routine
    begin = clock();
    if (file_cnt > 2) {
        show_help();
        custom_exit(ExitCode::FILE_NOT_FOUND);
    }
    process_file(nullptr, nullptr, max_file_size, g_force_zlib0_out);
    if (errorlevel.load() >= err_tresh) error_cnt++;
    if (errorlevel.load() == 1 ) warn_cnt++;
    if ( errorlevel.load() < err_tresh ) {
        acc_jpgsize += jpgfilesize;
        acc_ujgsize += ujgfilesize;
    }
    if (!g_use_seccomp) {
        end = clock();
    }
    if (action != socketserve && action != forkserve) {
        // show statistics
        fprintf(msgout,  "\n\n-> %i file(s) processed, %i error(s), %i warning(s)\n",
                file_cnt, error_cnt, warn_cnt);
    }
    if ( ( file_cnt > error_cnt ) && ( verbosity > 0 ) )
    if ( action == comp ) {
        speed = (int) ( (double) (( end - begin ) * 1000) / CLOCKS_PER_SEC );
        bpms  = ( speed > 0 ) ? ( acc_jpgsize / speed ) : acc_jpgsize;
        cr    = ( acc_jpgsize > 0 ) ? ( 100.0 * acc_ujgsize / acc_jpgsize ) : 0;

        fprintf( msgout,  " --------------------------------- \n" );
        fprintf( msgout,  " time taken        : %8i msec\n", speed );
        fprintf( msgout,  " avrg. byte per ms : %8i byte\n", bpms );
        fprintf( msgout,  " avrg. comp. ratio : %8.2f %%\n", cr );
        fprintf( msgout,  " --------------------------------- \n" );
    }
    return error_cnt == 0 ? 0 : 1;
}