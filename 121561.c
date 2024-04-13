int open_fdout(const char *ifilename,
                    IOUtil::FileWriter *writer,
               bool is_embedded_jpeg,
                    Sirikata::Array1d<uint8_t, 2> fileid,
                    bool force_compressed_output,
                    bool *is_socket) {
    if (writer != NULL) {
        *is_socket = writer->is_socket();
        return writer->get_fd();
    }
    *is_socket = false;
    if (strcmp(ifilename, "-") == 0) {
        return 1;
    }
    int retval = -1;
    std::string ofilename;
    // check file id, determine filetype
    if (file_no + 1 < file_cnt && ofilename != ifilename) {
        ofilename = filelist[file_no + 1];
    } else if (is_jpeg_header(fileid) || is_embedded_jpeg || g_permissive) {
        ofilename = postfix_uniq(ifilename, (ofiletype == UJG ? ".ujg" : ".lep"));
    } else if ( ( ( fileid[0] == ujg_header[0] ) && ( fileid[1] == ujg_header[1] ) )
                || ( ( fileid[0] == lepton_header[0] ) && ( fileid[1] == lepton_header[1] ) )
                || ( ( fileid[0] == zlepton_header[0] ) && ( fileid[1] == zlepton_header[1] ) ) ){
        if ((fileid[0] == zlepton_header[0] && fileid[1] == zlepton_header[1])
            || force_compressed_output) {
            ofilename = postfix_uniq(ifilename, ".jpg.z");
        } else {
            ofilename = postfix_uniq(ifilename, ".jpg");
        }
    }
    do {
        retval = open(ofilename.c_str(), O_WRONLY|O_CREAT|O_TRUNC
#ifdef _WIN32
            | O_BINARY
#endif
            , 0
#ifdef _WIN32
            | S_IREAD| S_IWRITE
#else
            | S_IWUSR | S_IRUSR
#endif
        );
    }while (retval == -1 && errno == EINTR);
    if (retval == -1) {
        const char * errormessage = "Output file unable to be opened for writing:";
        while(write(2, errormessage, strlen(errormessage)) == -1 && errno == EINTR) {}
        while(write(2, ofilename.c_str(), ofilename.length()) == -1 && errno == EINTR) {}
        while(write(2, "\n", 1) == -1 && errno == EINTR) {}
        custom_exit(ExitCode::FILE_NOT_FOUND);
    }
    return retval;
}