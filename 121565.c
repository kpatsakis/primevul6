void process_file(IOUtil::FileReader* reader,
                  IOUtil::FileWriter *writer,
                  int max_file_size,
                  bool force_zlib0)
{
    clock_t begin = 0, end = 1;
    const char* actionmsg  = NULL;
    const char* errtypemsg = NULL;
    int speed, bpms;
    float cr;


    if (g_inject_syscall_test == 2) {
        unsigned int num_workers = std::max(
            NUM_THREADS - 1,
            1U);
        GenericWorker* generic_workers = get_worker_threads(num_workers);
        if (g_inject_syscall_test == 2) {
            for (size_t i = 0; i < num_workers; ++i) {
                std::atomic<int> value;
                value.store(0);
                generic_workers[i].work = std::bind(&test_syscall_injection, &value);
                generic_workers[i].activate_work();
                generic_workers[i].instruct_to_exit();
                generic_workers[i].join_via_syscall();
                if (value.load() < 1) {
                    abort(); // this should exit_group
                }
            }
            g_threaded = false;
        }
    }
    // main function routine
    errorlevel.store(0);
    jpgfilesize = 0;
    ujgfilesize = 0;

    Sirikata::Array1d<uint8_t, 2> header = {{0, 0}};
    const char * ifilename = filelist[file_no];
    bool is_socket = false;
    ssize_t bytes_read =0 ;
    int fdin = open_fdin(ifilename, reader, header, &bytes_read, &is_socket);
    /*
    if (g_permissive && bytes_read < 2) {
        std::vector<uint8_t> input(bytes_read);
        if (bytes_read > 0) {
            memcpy(&input[0], header.data, bytes_read);
        }
        Sirikata::MuxReader::ResizableByteBuffer lepton_data;
        ExitCode exit_code = ExitCode::UNSUPPORTED_JPEG;
        ValidationContinuation validation_exit_code = generic_compress(&input, &lepton_data, &exit_code);
        if (exit_code != ExitCode::SUCCESS) {
            custom_exit(exit_code);
        }
        if (validation_exit_code != ValidationContinuation::ROUNDTRIP_OK) {
            custom_exit(ExitCode::UNSUPPORTED_JPEG);
        }
        int fdout = open_fdout(ifilename, writer, true, header, g_force_zlib0_out || force_zlib0, &is_socket);
        for (size_t data_sent = 0; data_sent < lepton_data.size();) {
            ssize_t sent = write(fdout,
                                 lepton_data.data() + data_sent,
                                 lepton_data.size() - data_sent);
            if (sent < 0 && errno == EINTR){
                continue;
            }
            if (sent <= 0) {
                custom_exit(ExitCode::SHORT_READ);
            }
            data_sent += sent;
        }
        //fprintf(stderr, "OK...\n");
        custom_exit(ExitCode::SUCCESS);
        
        }*/
    int fdout = -1;
    if ((embedded_jpeg || is_jpeg_header(header) || g_permissive) && (g_permissive ||  !g_skip_validation)) {
        //fprintf(stderr, "ENTERED VALIDATION...\n");
        ExitCode validation_exit_code = ExitCode::SUCCESS;
        Sirikata::MuxReader::ResizableByteBuffer lepton_data;
        std::vector<uint8_t> permissive_jpeg_return_backing;
        switch (validateAndCompress(&fdin, &fdout, header,
                                    bytes_read,
                                    start_byte, max_file_size,
                                    &validation_exit_code,
                                    &lepton_data,
                                    g_argc,
                                    g_argv,
                                    g_permissive,
                                    is_socket,
                                    g_permissive? &permissive_jpeg_return_backing:NULL)) {
          case ValidationContinuation::CONTINUE_AS_JPEG:
            //fprintf(stderr, "CONTINUE AS JPEG...\n");
            is_socket = false;
            break;
          case ValidationContinuation::CONTINUE_AS_LEPTON:
            embedded_jpeg = false;
            is_socket = false;
            g_force_zlib0_out = false;
            force_zlib0 = false;
            if (ofiletype ==  UJG) {
                filetype = UJG;
                header[0] = ujg_header[0];
                header[1] = ujg_header[1];
            } else {
                filetype = LEPTON;
                header[0] = lepton_header[0];
                header[1] = lepton_header[1];
            }
            //fprintf(stderr, "CONTINUE AS LEPTON...\n");
            break;
        case ValidationContinuation::EVALUATE_AS_PERMISSIVE:
            if (permissive_jpeg_return_backing.size() == 0) {
                custom_exit(ExitCode::UNSUPPORTED_JPEG);
            }
            fdout = open_fdout(ifilename, writer, embedded_jpeg, header, g_force_zlib0_out || force_zlib0, &is_socket);
            {ExitCode validation_exit_code = ExitCode::UNSUPPORTED_JPEG;
            generic_compress(&permissive_jpeg_return_backing, &lepton_data, &validation_exit_code);
            if (validation_exit_code != ExitCode::SUCCESS) {
                custom_exit(validation_exit_code);
            }}
            for (size_t data_sent = 0; data_sent < lepton_data.size();) {
                ssize_t sent = write(fdout,
                                     lepton_data.data() + data_sent,
                                     lepton_data.size() - data_sent);
                if (sent < 0 && errno == EINTR){
                    continue;
                }
                if (sent <= 0) {
                    custom_exit(ExitCode::SHORT_READ);
                }
                data_sent += sent;
            }
            //fprintf(stderr, "OK...\n");
            custom_exit(ExitCode::SUCCESS);
            break;
        case ValidationContinuation::ROUNDTRIP_OK:
            fdout = open_fdout(ifilename, writer, embedded_jpeg, header, g_force_zlib0_out || force_zlib0, &is_socket);
            for (size_t data_sent = 0; data_sent < lepton_data.size();) {
                ssize_t sent = write(fdout,
                                     lepton_data.data() + data_sent,
                                     lepton_data.size() - data_sent);
                if (sent < 0 && errno == EINTR){
                    continue;
                }
                if (sent <= 0) {
                    custom_exit(ExitCode::SHORT_READ);
                }
                data_sent += sent;
            }
            //fprintf(stderr, "OK...\n");
            custom_exit(ExitCode::SUCCESS);
          case ValidationContinuation::BAD:
          default:
            always_assert(validation_exit_code != ExitCode::SUCCESS);
            custom_exit(validation_exit_code);
        }        
    } else {
        if (action != lepton_concatenate) {
            fdout = open_fdout(ifilename, writer, embedded_jpeg, header, g_force_zlib0_out || force_zlib0, &is_socket);
        }
    }
    if (action == lepton_concatenate) {
        concatenate_files(fdin, fdout);
        return;
    }
    // check input file and determine filetype
    check_file(fdin, fdout, max_file_size, force_zlib0, embedded_jpeg, header, is_socket);
    
    begin = clock();
    if ( filetype == JPEG )
    {


        if (ofiletype == LEPTON) {
            if (!g_encoder) {
                if (ujgversion == 3) {
#ifdef ENABLE_ANS_EXPERIMENTAL
                    g_encoder.reset(makeEncoder<ANSBoolReader>(g_threaded, g_threaded));
#else
                    always_assert(false&&"ANS-encoded file encountered but ANS not selected in build flags");
#endif
                } else {
                    g_encoder.reset(makeEncoder<VPXBoolReader>(g_threaded, g_threaded));
                }
                TimingHarness::timing[0][TimingHarness::TS_MODEL_INIT] = TimingHarness::get_time_us();
                g_decoder = NULL;
            } else if (g_threaded && (action == socketserve || action == forkserve)) {
                g_encoder->registerWorkers(get_worker_threads(NUM_THREADS - 1), NUM_THREADS  - 1);
            }
        }else if (ofiletype == UJG) {
            g_encoder.reset(new SimpleComponentEncoder);
            g_decoder = NULL;
        }
    } else if (filetype == LEPTON) {
        NUM_THREADS = read_fixed_ujpg_header();
        if (NUM_THREADS == 1) {
            g_threaded = false; // with singlethreaded, doesn't make sense to split out reader/writer
        }
        if (!g_decoder) {
            g_decoder = makeDecoder(g_threaded, g_threaded, ujgversion == 3);
            TimingHarness::timing[0][TimingHarness::TS_MODEL_INIT] = TimingHarness::get_time_us();
            g_reference_to_free.reset(g_decoder);
        } else if (NUM_THREADS > 1 && g_threaded && (action == socketserve || action == forkserve)) {
            g_decoder->registerWorkers(get_worker_threads(NUM_THREADS), NUM_THREADS);
        }
    }else if (filetype == UJG) {
        (void)read_fixed_ujpg_header();
        g_decoder = new SimpleComponentDecoder;
        g_reference_to_free.reset(g_decoder);
    }
#ifndef _WIN32
    //FIXME
    if (g_time_bound_ms) {
        struct itimerval bound;
        bound.it_value.tv_sec = g_time_bound_ms / 1000;
        bound.it_value.tv_usec = (g_time_bound_ms % 1000) * 1000;
        bound.it_interval.tv_sec = 0;
        bound.it_interval.tv_usec = 0;
        int ret = setitimer(ITIMER_REAL, &bound, NULL);

        dev_assert(ret == 0 && "Timer must be able to be set");
        if (ret != 0) {
            exit((int)ExitCode::OS_ERROR);
        }
    }
#endif
    if (g_unkillable) { // only set this after the time bound has been set
        if (!g_time_bound_ms) {
            fprintf(stderr, "Only allowed to set unkillable for items with a time bound\n");
            exit(1);
        }
        signal(SIGTERM, &sig_nop);
#ifndef _WIN32
        signal(SIGQUIT, &sig_nop);
#endif
    }

    if (g_use_seccomp) {
        Sirikata::installStrictSyscallFilter(true);
    }
#ifndef _WIN32
    if (g_inject_syscall_test == 1) {
        char buf[128 + 1];
        buf[sizeof(buf) - 1] = 0;
        char * ret = getcwd(buf, sizeof(buf) - 1);
        (void)ret;
    }
#endif
    // get specific action message
    if ( filetype == UNK ) {
        actionmsg = "unknown filetype";
    } else if (action == info) {
        actionmsg = "Parsing";
    } else if ( filetype == JPEG ) {
        actionmsg = "Writing to LEPTON\n";
    } else {
        actionmsg = "Decompressing to JPEG\n";
    }

    if ( verbosity > 0 ) {
        while (write(2, actionmsg , strlen(actionmsg)) < 0 && errno == EINTR) {}
    }


    std::vector<std::pair<uint32_t, uint32_t> > huff_input_offset;
    if ( filetype == JPEG )
    {
        switch ( action )
        {
            case lepton_concatenate:
              fprintf(stderr, "Unable to concatenate raw JPEG files together\n");
              custom_exit(ExitCode::VERSION_UNSUPPORTED);
              break;
            case comp:
            case forkserve:
            case socketserve:
                timing_operation_start( 'c' );
                TimingHarness::timing[0][TimingHarness::TS_READ_STARTED] = TimingHarness::get_time_us();
                {
                    std::vector<uint8_t,
                                Sirikata::JpegAllocator<uint8_t> > jpeg_file_raw_bytes;
                    unsigned int jpg_ident_offset = 2;
                    if (start_byte == 0) {
                        ibytestream str_jpg_in(str_in,
                                               jpg_ident_offset,
                                               Sirikata::JpegAllocator<uint8_t>());

                        execute(std::bind(&read_jpeg_wrapper, &huff_input_offset, &str_jpg_in, header, embedded_jpeg));
                    } else {
                        ibytestreamcopier str_jpg_in(str_in,
                                                     jpg_ident_offset,
                                                     max_file_size,
                                                     Sirikata::JpegAllocator<uint8_t>());
                        str_jpg_in.mutate_read_data().push_back(0xff);
                        str_jpg_in.mutate_read_data().push_back(0xd8);
                        execute(std::bind(&read_jpeg_and_copy_to_side_channel,
                                          &huff_input_offset, &str_jpg_in, header,
                                          embedded_jpeg));
                        jpeg_file_raw_bytes.swap(str_jpg_in.mutate_read_data());
                    }
                    TimingHarness::timing[0][TimingHarness::TS_JPEG_DECODE_STARTED] =
                        TimingHarness::timing[0][TimingHarness::TS_READ_FINISHED] = TimingHarness::get_time_us();
                    std::vector<ThreadHandoff> luma_row_offsets;
                    execute(std::bind(&decode_jpeg, huff_input_offset, &luma_row_offsets));
                    TimingHarness::timing[0][TimingHarness::TS_JPEG_DECODE_FINISHED]
                        = TimingHarness::get_time_us();
                    //execute( check_value_range );
                    execute(std::bind(&write_ujpg,
                                      std::move(luma_row_offsets),
                                      jpeg_file_raw_bytes.empty() ? NULL : &jpeg_file_raw_bytes));
                }
                timing_operation_complete( 'c' );
                break;

            case info:
                {
                    unsigned int jpg_ident_offset = 2;
                    ibytestream str_jpg_in(str_in, jpg_ident_offset, Sirikata::JpegAllocator<uint8_t>());
                    execute(std::bind(read_jpeg_wrapper, &huff_input_offset, &str_jpg_in, header,
                                      embedded_jpeg));
                }
                execute( write_info );
                break;
        }
    }
    else if ( filetype == UJG || filetype == LEPTON)
    {
        switch ( action )
        {
            case lepton_concatenate:
              always_assert(false && "should have been handled above");
            case comp:
            case forkserve:
            case socketserve:
                if (!g_use_seccomp) {
                    overall_start = clock();
                }
                timing_operation_start( 'd' );
                TimingHarness::timing[0][TimingHarness::TS_READ_STARTED] = TimingHarness::get_time_us();
                while (true) {
                    execute( read_ujpg ); // replace with decompression function!
                    TimingHarness::timing[0][TimingHarness::TS_READ_FINISHED] = TimingHarness::get_time_us();
                    if (!g_use_seccomp) {
                        read_done = clock();
                    }
                    TimingHarness::timing[0][TimingHarness::TS_JPEG_RECODE_STARTED] = TimingHarness::get_time_us();
                    if (filetype != UJG && !g_allow_progressive) {
                        execute(recode_baseline_jpeg_wrapper);
                    } else {
                        execute(recode_jpeg);
                    }
                    timing_operation_complete( 'd' );
                    TimingHarness::timing[0][TimingHarness::TS_JPEG_RECODE_FINISHED] = TimingHarness::get_time_us();
                    Sirikata::Array1d<uint8_t, 6> trailer_new_header;
                    std::pair<uint32_t, Sirikata::JpegError> continuity;
                    size_t off = 0;
                    while (off < trailer_new_header.size()) {
                        continuity = str_in->Read(&trailer_new_header[off], trailer_new_header.size() - off);
                        off += continuity.first;
                        if (continuity.second != Sirikata::JpegError::nil()) {
                            break;
                        }
                    }
                    if (continuity.second != Sirikata::JpegError::nil()) {
                        break;
                    } else if (trailer_new_header[4] != header[0] ||  trailer_new_header[5] != header[1]) {
                        break;
                    } else {
                        prep_for_new_file();
                    }
                }
                str_out->close();
                break;
            case info:
                execute( read_ujpg );
                execute( write_info );
                break;
        }
    }
    if (!fast_exit) {
        // close iostreams
        if ( str_in  != NULL ) delete( str_in  ); str_in  = NULL;
        if ( str_out != NULL ) delete( str_out ); str_out = NULL;
        if ( ujg_out != NULL ) delete( ujg_out ); ujg_out = NULL;
        // delete if broken or if output not needed
        if ((!pipe_on) && ((errorlevel.load() >= err_tresh)
                           || (action != comp && action != forkserve && action != socketserve))) {
            // FIXME: can't delete broken output--it's gone already
        }
    }
    TimingHarness::timing[0][TimingHarness::TS_DONE] = TimingHarness::get_time_us();
    TimingHarness::print_results();
    if (!g_use_seccomp) {
        end = clock();
    }
    {
        size_t bound = decompression_memory_bound();
        char bound_out[] = "XXXXXXXXXX bytes needed to decompress this file\n";
        bound_out[0] = '0' + (bound / 1000000000)%10;
        bound_out[1] = '0' + (bound / 100000000)%10;
        bound_out[2] = '0' + (bound / 10000000)%10;
        bound_out[3] = '0' + (bound / 1000000)%10;
        bound_out[4] = '0' + (bound / 100000)%10;
        bound_out[5] = '0' + (bound / 10000)%10;
        bound_out[6] = '0' + (bound / 1000)%10;
        bound_out[7] = '0' + (bound / 100)%10;
        bound_out[8] = '0' + (bound / 10)%10;
        bound_out[9] = '0' + (bound / 1)%10;
        const char * to_write = bound_out;
        while(to_write[0] == '0') {
            ++to_write;
        }
        while(write(2, to_write, strlen(to_write)) < 0 && errno == EINTR) {
        }
    }
    print_bill(2);
    // speed and compression ratio calculation
    speed = (int) ( (double) (( end - begin ) * 1000) / CLOCKS_PER_SEC );
    bpms  = ( speed > 0 ) ? ( jpgfilesize / speed ) : jpgfilesize;
    cr    = ( jpgfilesize > 0 ) ? ( 100.0 * ujgfilesize / jpgfilesize ) : 0;

    switch ( verbosity )
    {
        case 0:
          if ( errorlevel.load() < err_tresh ) {
                if (action == comp ) {
                    fprintf(stderr, "%d %d\n",(int)ujgfilesize, (int)jpgfilesize);
                    char percentage_report[]=" XX.XX%\n";
                    double pct = cr + .005;
                    percentage_report[0] = '0' + (int)(pct / 100) % 10;
                    percentage_report[1] = '0' + (int)(pct / 10) % 10;
                    percentage_report[2] = '0' + (int)(pct) % 10;
                    percentage_report[4] = '0' + (int)(pct * 10) % 10;
                    percentage_report[5] = '0' + (int)(pct * 100) % 10;
                    char * output = percentage_report;
                    if (cr < 100) {
                        ++output;
                    }
                    while (write(2, output, strlen(output)) < 0 && errno == EINTR) {
                    }
                }
                else {
                    fprintf( msgout,  "DONE\n" );
                }
            }
            break;

        case 1:
          if ( errorlevel.load() < err_tresh ) fprintf( msgout,  "DONE\n" );
            else fprintf( msgout,  "ERROR\n" );
            break;

        case 2:
            fprintf( msgout,  "\n----------------------------------------\n" );
            if ( errorlevel.load() < err_tresh ) fprintf( msgout,  "-> %s OK\n", actionmsg );
            break;
    }

    switch ( errorlevel.load() )
    {
        case 0:
            errtypemsg = "none";
            break;

        case 1:
            if ( errorlevel.load() < err_tresh )
                errtypemsg = "warning (ignored)";
            else
                errtypemsg = "warning (skipped file)";
            break;

        case 2:
            errtypemsg = "fatal error";
            break;
    }

    if ( errorlevel.load() > 0 )
    {
        if (false && action != socketserve && action != forkserve) {
            fprintf( stderr, " %s:\n", errtypemsg  );
            fprintf( stderr, " %s\n", errormessage.c_str() );
            if ( verbosity > 1 )
                fprintf( stderr, " (in file \"%s\")\n", filelist[ file_no ] );
        }
    }
    if ( (verbosity > 0) && (errorlevel.load() < err_tresh) )
    if ( action == comp )
    {
        fprintf( msgout,  " time taken  : %7i msec\n", speed );
        fprintf( msgout,  " byte per ms : %7i byte\n", bpms );
        fprintf( msgout,  " comp. ratio : %7.2f %%\n", cr );
    }

    if ( ( verbosity > 1 ) && ( action == comp ) )
        fprintf( msgout,  "\n" );
    LeptonDebug::dumpDebugData();
    if (errorlevel.load()) {
        custom_exit(ExitCode::UNSUPPORTED_JPEG); // custom exit will delete generic_workers
    } else {
        custom_exit(ExitCode::SUCCESS);
    }
    reset_buffers();
}