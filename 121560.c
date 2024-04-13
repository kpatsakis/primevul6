unsigned char read_fixed_ujpg_header() {
    Sirikata::Array1d<unsigned char, 22> header;
    header.memset(0);

    if (IOUtil::ReadFull(str_in, header.begin(), 22) != 22) {
        custom_exit(ExitCode::SHORT_READ);
    }
    // check version number
    if (header[0] != 1 && header[0] != 2 && header[0] != 3 && header[0] != 4 && header[0] != ujgversion) {
        // let us roll out a new version gently
        fprintf( stderr, "incompatible file, use %s v%i.%i",
            appname, header[ 0 ] / 10, header[ 0 ] % 10 );
        custom_exit(ExitCode::VERSION_UNSUPPORTED);
    }
    ujgversion = header[0];
    if (header[1] == 'X') {
    } else if (header[1] != 'Z' && header[1] != 'Y') {
        char err[] = "?: Unknown Item in header instead of Z";
        err[0] = header[1];
        while(write(2, err, sizeof(err) - 1) < 0 && errno == EINTR) {
        }
    }
    if (header[1] == 'Z' || (header[1] & 1) == ('Y' & 1)) {
        if (!g_force_progressive) {
            g_allow_progressive = false;
        }
    }
    unsigned char num_threads_hint = header[2];
    always_assert(num_threads_hint != 0);
    if (num_threads_hint < NUM_THREADS && num_threads_hint != 0) {
        NUM_THREADS = num_threads_hint;
    }
// full size of the original file
    Sirikata::Array1d<unsigned char, 4>::Slice file_size = header.slice<18,22>();
    max_file_size = LEtoUint32(file_size.begin());
    return NUM_THREADS;
}