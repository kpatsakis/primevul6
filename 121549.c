bool check_file(int fd_in, int fd_out, uint32_t max_file_size, bool force_zlib0,
                bool is_embedded_jpeg, Sirikata::Array1d<uint8_t, 2> fileid, bool is_socket)
{
    IOUtil::FileReader * reader = IOUtil::BindFdToReader(fd_in, max_file_size, is_socket);
    if (!reader) {
        custom_exit(ExitCode::FILE_NOT_FOUND);
    }
    reader->mark_some_bytes_already_read((uint32_t)fileid.size());
    if (is_socket) {
        dev_assert(fd_in == fd_out);
    }
    IOUtil::FileWriter * writer = IOUtil::BindFdToWriter(fd_out == -1 ? 1 /* stdout */ : fd_out, is_socket);
    ujg_base_in = reader;
    // check file id, determine filetype
    if (is_embedded_jpeg || is_jpeg_header(fileid)) {
        str_in = new Sirikata::BufferedReader<JPG_READ_BUFFER_SIZE>(reader);
        // file is JPEG
        filetype = JPEG;
        NUM_THREADS = std::min(NUM_THREADS, (unsigned int)max_encode_threads);
        // open output stream, check for errors
        ujg_out = writer;
    }
    else if ( ( ( fileid[0] == ujg_header[0] ) && ( fileid[1] == ujg_header[1] ) )
              || ( ( fileid[0] == lepton_header[0] ) && ( fileid[1] == lepton_header[1] ) )
              || ( ( fileid[0] == zlepton_header[0] ) && ( fileid[1] == zlepton_header[1] ) ) ){
        str_in = reader;
        bool compressed_output = (fileid[0] == zlepton_header[0]) && (fileid[1] == zlepton_header[1]);
        compressed_output = compressed_output || g_force_zlib0_out || force_zlib0;
        // file is UJG
        filetype = (( fileid[0] == ujg_header[0] ) && ( fileid[1] == ujg_header[1] ) ) ? UJG : LEPTON;
        std::function<void(Sirikata::DecoderWriter*, size_t file_size)> known_size_callback = &nop;
        Sirikata::DecoderWriter * write_target = writer;
        if (compressed_output) {
            Sirikata::Zlib0Writer * zwriter;
            if (uninit_g_zlib_0_writer) {
                zwriter = new(uninit_g_zlib_0_writer)Sirikata::Zlib0Writer(writer, 0);
                uninit_g_zlib_0_writer = NULL;
            }else {
                zwriter = new Sirikata::Zlib0Writer(writer, 0);
            }
            known_size_callback = &nop;
            write_target = zwriter;
        }
        str_out = new bounded_iostream( write_target,
                                        known_size_callback,
                                        Sirikata::JpegAllocator<uint8_t>());
        if ( str_out->chkerr() ) {
            fprintf( stderr, FWR_ERRMSG, filelist[file_no]);
            errorlevel.store(2);
            return false;
        }
    }
    else {
        // file is neither
        filetype = UNK;
        fprintf( stderr, "filetype of file \"%s\" is unknown", filelist[ file_no ] );
        errorlevel.store(2);
        return false;
    }


    return true;
}