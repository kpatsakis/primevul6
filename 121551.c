int initialize_options( int argc, const char*const * argv )
{
    const char** tmp_flp;
    int tmp_val;
    int max_file_size = 0;
    // get memory for filelist & preset with NULL
    filelist = (const char**)custom_calloc(argc * sizeof(char*));

    // preset temporary filelist pointer
    tmp_flp = filelist;
    // read in arguments
    while ( --argc > 0 ) {
        argv++;
        // switches begin with '-'
        if ( sscanf( (*argv), "-v%i", &tmp_val ) == 1 ){
            verbosity = tmp_val;
            verbosity = ( verbosity < 0 ) ? 0 : verbosity;
            verbosity = ( verbosity > 2 ) ? 2 : verbosity;
        }
        else if ( strcmp((*argv), "-o" ) == 0 ) {
            overwrite = true;
        }
        else if (strcmp((*argv), "-revision" ) == 0 || strcmp((*argv), "--revision") == 0) {
            printf("%s\n", GIT_REVISION);
            exit(0);
        }
        else if (strcmp((*argv), "-version" ) == 0 || strcmp((*argv), "--version") == 0) {
            printf("%02x\n", ujgversion);
            exit(0);
        } else if ( strcmp((*argv), "-preload" ) == 0 ) {
            g_do_preload = true;
        } else if ( strcmp((*argv), "-decode" ) == 0 ) { // deprecated commands to preload it all
            g_do_preload = true;
        } else if ( strcmp((*argv), "-recode" ) == 0 ) {
            g_do_preload = true;
        } else if ( strcmp((*argv), "-p" ) == 0 ) {
            err_tresh = 2;
        }
        else if ( strncmp((*argv), "-timebound=", strlen("-timebound=")) == 0) {
            char * endptr = NULL;
            g_time_bound_ms = strtoll((*argv) + strlen("-timebound="), &endptr, 10);
            if (endptr) {
                if (strcmp(endptr, "s") == 0) {
                    g_time_bound_ms *= 1000;
                } else if (strcmp(endptr, "us") == 0) {
                    g_time_bound_ms /= 1000;
                } else if (strcmp(endptr, "ms") != 0) {
                    fprintf(stderr, "Time must have units (ms or s)\n");
                    exit(1);
                }
            }
        }
        else if ( strcmp((*argv), "-zlib0" ) == 0)  {
            g_force_zlib0_out = true;
        }
        else if ( strcmp((*argv), "-unkillable" ) == 0)  {
            g_unkillable = true;
        }
        else if ( strcmp((*argv), "-singlethread" ) == 0)  {
            g_threaded = false;
        }
        else if ( strcmp((*argv), "-allowprogressive" ) == 0)  {
            g_allow_progressive = true;
        }
        else if ( strcmp((*argv), "-forceprogressive" ) == 0)  {
            g_allow_progressive = true;
            g_force_progressive = true;
        }
        else if ( strcmp((*argv), "-rejectprogressive" ) == 0)  {
            g_allow_progressive = false;
        }
        else if ( strcmp((*argv), "-unjailed" ) == 0)  {
            g_use_seccomp = false;
        } else if ( strcmp((*argv), "-multithread" ) == 0 || strcmp((*argv), "-m") == 0)  {
            g_threaded = true;
        } else if ( strcmp((*argv), "-evensplit" ) == 0)  {
            g_even_thread_split = true;
        } else if ( strstr((*argv), "-recodememory=") == *argv ) {
            g_decompression_memory_bound
                = local_atoi(*argv + strlen("-recodememory="));
        } else if ( strstr((*argv), "-memory=") == *argv ) {

        } else if ( strstr((*argv), "-hugepages") == *argv ) {

        } else if ( strstr((*argv), "-defermd5") == *argv ) {

        } else if ( strstr((*argv), "-avx2upgrade") == *argv ) {

        } else if ( strstr((*argv), "-threadmemory=") == *argv ) {

        } else if ( strncmp((*argv), "-timing=", strlen("-timing=") ) == 0 ) {
            timing_log = fopen((*argv) + strlen("-timing="), "a");
        } else if (strncmp((*argv), "-maxencodethreads=", strlen("-maxencodethreads=") ) == 0 ) {
            max_encode_threads = local_atoi((*argv) + strlen("-maxencodethreads="));
            if (max_encode_threads > MAX_NUM_THREADS) {
                custom_exit(ExitCode::VERSION_UNSUPPORTED);
            }
        } else if (strcmp((*argv), "-lepcat") == 0) {
            action = lepton_concatenate;
        } else if (strncmp((*argv), "-minencodethreads=", strlen("-minencodethreads=") ) == 0 ) {
            min_encode_threads = local_atoi((*argv) + strlen("-minencodethreads="));
        } else if ( strncmp((*argv), "-injectsyscall=", strlen("-injectsyscall=") ) == 0 ) {
            g_inject_syscall_test = strtol((*argv) + strlen("-injectsyscall="), NULL, 10);
        } else if ( strcmp((*argv), "-skipvalidation") == 0 ) {
            g_skip_validation = true;
        } else if ( strcmp((*argv), "-skipvalidate") == 0 ) {
            g_skip_validation = true;
        } else if ( strcmp((*argv), "-skipverify") == 0 ) {
            g_skip_validation = true;
        } else if ( strcmp((*argv), "-skipverification") == 0 ) {
            g_skip_validation = true;
        } else if ( strcmp((*argv), "-skiproundtrip") == 0 ) {
            g_skip_validation = true;
        } else if ( strcmp((*argv), "-validate") == 0 ) {
            g_skip_validation = false;
        } else if ( strcmp((*argv), "-validation") == 0 ) {
            g_skip_validation = false;
        } else if ( strcmp((*argv), "-verify") == 0 ) {
            g_skip_validation = false;
        } else if ( strcmp((*argv), "-verification") == 0 ) {
            g_skip_validation = false;
        } else if ( strcmp((*argv), "-roundtrip") == 0 ) {
            g_skip_validation = false;
        } else if ( strcmp((*argv), "-permissive") == 0 ) {
            g_permissive = true;
#ifndef _WIN32
            signal(SIGPIPE, SIG_IGN);
#endif
        } else if ( strcmp((*argv), "-brotliheader") == 0 ) {
            if (ujgversion < 2) {
                ujgversion = 2; // use brotli to compress the header and trailer rather than zlib
            }
        } else if ( strcmp((*argv), "-ans") == 0 ) {
#ifdef ENABLE_ANS_EXPERIMENTAL
            ujgversion = 3; // use brotli to compress the header and trailer rather than zlib and ANS encoder/decoder
#else
            always_assert(false && "ANS selected via command line but not enabled in build flags");
#endif          
        } else if ( strncmp((*argv), "-maxchildren=", strlen("-maxchildren=") ) == 0 ) {
            g_socketserve_info.max_children = strtol((*argv) + strlen("-maxchildren="), NULL, 10);
        }
        else if ( strncmp((*argv), "-listenbacklog=", strlen("-listenbacklog=") ) == 0 ) {
            g_socketserve_info.listen_backlog = strtol((*argv) + strlen("-listenbacklog="), NULL, 10);
        }
        else if ( strncmp((*argv), "-startbyte=", strlen("-startbyte=") ) == 0 ) {
            start_byte = local_atoi((*argv) + strlen("-startbyte="));
        }        
        else if ( strncmp((*argv), "-embedding=", strlen("-embedding=") ) == 0 ) {
            jpeg_embedding_offset = local_atoi((*argv) + strlen("-embedding="));
            embedded_jpeg = true;
        }
        else if ( strncmp((*argv), "-trunc=", strlen("-trunc=") ) == 0 ) {
            max_file_size = local_atoi((*argv) + strlen("-trunc="));
        }
        else if ( strncmp((*argv), "-trunctiming=", strlen("-trunctiming=") ) == 0 ) {
            timing_log = fopen((*argv) + strlen("-trunctiming="), "w");
        }
        else if ( strcmp((*argv), "-d" ) == 0 ) {
            disc_meta = true;
        }
        else if ( strcmp((*argv), "-dev") == 0 ) {
            developer = true;
        } else if ( ( strcmp((*argv), "-ujg") == 0 ) ||
                    ( strcmp((*argv), "-ujpg") == 0 )) {
            fprintf(stderr, "FOUND UJG ARG: using that as output\n");
            action = comp;
            ofiletype = UJG;
#ifndef _WIN32
        } else if ( strcmp((*argv), "-fork") == 0 ) {    
            action = forkserve;
            // sets it up in serving mode
            msgout = stderr;
            // use "-" as placeholder for the socket
            *(tmp_flp++) = g_dash;
        } else if ( strncmp((*argv), "-socket", strlen("-socket")) == 0 ) {
            if (action != socketserve) {
                action = socketserve;
                // sets it up in serving mode
                msgout = stderr;
                // use "-" as placeholder for the socket
                *(tmp_flp++) = g_dash;
            }
            if ((*argv)[strlen("-socket")] == '=') {
                g_socketserve_info.uds = (*argv) + strlen("-socket=");
            }
        } else if ( strncmp((*argv), "-listen", strlen("-listen")) == 0 ) {
            g_socketserve_info.listen_tcp = true;
            if (action != socketserve) {
                action = socketserve;
                // sets it up in serving mode
                msgout = stderr;
                // use "-" as placeholder for the socket
                *(tmp_flp++) = g_dash;
            }
            if ((*argv)[strlen("-listen")] == '=') {
                g_socketserve_info.port = atoi((*argv) + strlen("-listen="));
            }
        } else if ( strncmp((*argv), "-zliblisten", strlen("-zliblisten")) == 0 ) {
            g_socketserve_info.zlib_port = atoi((*argv) + strlen("-zliblisten="));
#endif
        } else if ( strcmp((*argv), "-") == 0 ) {    
            msgout = stderr;
            // set binary mode for stdin & stdout
            #ifdef _WIN32
                setmode( fileno( stdin ), O_BINARY );
                setmode( fileno( stdout ), O_BINARY );
            #endif
            // use "-" as placeholder for stdin
            *(tmp_flp++) = g_dash;
        }
        else {
            // if argument is not switch, it's a filename
            *(tmp_flp++) = *argv;
        }
    }
    for ( file_cnt = 0; filelist[ file_cnt ] != NULL; file_cnt++ ) {
    }
    if (start_byte != 0) {
        // Encode of partial progressive images not allowed
        g_allow_progressive = false;
    }
    if (g_time_bound_ms && action == forkserve) {
        fprintf(stderr, "Time bound action only supported with UNIX domain sockets\n");
        exit(1);
    }
    if (g_do_preload && g_skip_validation) {
        VP8ComponentDecoder<VPXBoolReader> *d = makeBoth<VPXBoolReader>(g_threaded, g_threaded && action != forkserve && action != socketserve);
        g_encoder.reset(d);
        g_decoder = d;
    }
    return max_file_size;
}