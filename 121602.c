int open_fdin(const char *ifilename,
              IOUtil::FileReader *reader,
              Sirikata::Array1d<uint8_t, 2> &header,
              ssize_t *bytes_read,
              bool *is_socket) {
    int fdin = -1;    
    if (reader != NULL) {
        *is_socket = reader->is_socket();
        fdin = reader->get_fd();
    }
    else if (strcmp(ifilename, "-") == 0) {
        fdin = 0;
        *is_socket = false;
    }
    else {
        *is_socket = false;
         do {
            fdin = open(ifilename, O_RDONLY
#ifdef _WIN32
                |O_BINARY
#endif
            );
        } while (fdin == -1 && errno == EINTR);
        if (fdin == -1) {
            const char * errormessage = "Input file unable to be opened for writing:";
            while(write(2, errormessage, strlen(errormessage)) == -1 && errno == EINTR) {}
            while(write(2, ifilename, strlen(ifilename)) == -1 && errno == EINTR) {}
            while(write(2, "\n", 1) == -1 && errno == EINTR) {}
        }
    }
    *bytes_read = 0;
    ssize_t data_read = 0;
    do {
        data_read = read(fdin, &header[0], 2);
    } while (data_read == -1 && errno == EINTR);
    if (data_read >= 0) {
        *bytes_read = data_read;
    }
    if (__builtin_expect(data_read < 2, false)) {
        do {
            data_read = read(fdin, &header[1], 1);
        } while (data_read == -1 && errno == EINTR);
        if (data_read >= 0) {
            *bytes_read += data_read;
        }
    }
    if (data_read < 0) {
        perror("read");
        const char * fail = "Failed to read 2 byte header\n";
        while(write(2, fail, strlen(fail)) == -1 && errno == EINTR) {}        
    }
    return fdin;
}