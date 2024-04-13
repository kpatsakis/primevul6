curses_trace(unsigned tracelevel)
{
    unsigned result;
#if defined(TRACE)
    result = _nc_tracing;
    if ((MyFP == 0) && tracelevel) {
	MyInit = TRUE;
	if (MyFD >= 0) {
	    MyFP = fdopen(MyFD, BIN_W);
	} else {
	    if (MyPath[0] == '\0') {
		size_t size = sizeof(MyPath) - 12;
		if (getcwd(MyPath, size) == 0) {
		    perror("curses: Can't get working directory");
		    exit(EXIT_FAILURE);
		}
		MyPath[size] = '\0';
		assert(strlen(MyPath) <= size);
		_nc_STRCAT(MyPath, "/trace", sizeof(MyPath));
		if (_nc_is_dir_path(MyPath)) {
		    _nc_STRCAT(MyPath, ".log", sizeof(MyPath));
		}
	    }
	    if (_nc_access(MyPath, W_OK) < 0
		|| (MyFD = open(MyPath, O_CREAT | O_EXCL | O_RDWR, 0600)) < 0
		|| (MyFP = fdopen(MyFD, BIN_W)) == 0) {
		;		/* EMPTY */
	    }
	}
	_nc_tracing = tracelevel;
	/* Try to set line-buffered mode, or (failing that) unbuffered,
	 * so that the trace-output gets flushed automatically at the
	 * end of each line.  This is useful in case the program dies. 
	 */
	if (MyFP != 0) {
#if HAVE_SETVBUF		/* ANSI */
	    (void) setvbuf(MyFP, (char *) 0, _IOLBF, (size_t) 0);
#elif HAVE_SETBUF /* POSIX */
	    (void) setbuffer(MyFP, (char *) 0);
#endif
	}
	_tracef("TRACING NCURSES version %s.%d (tracelevel=%#x)",
		NCURSES_VERSION,
		NCURSES_VERSION_PATCH,
		tracelevel);
    } else if (tracelevel == 0) {
	if (MyFP != 0) {
	    MyFD = dup(MyFD);	/* allow reopen of same file */
	    fclose(MyFP);
	    MyFP = 0;
	}
	_nc_tracing = tracelevel;
    } else if (_nc_tracing != tracelevel) {
	_nc_tracing = tracelevel;
	_tracef("tracelevel=%#x", tracelevel);
    }
#else
    (void) tracelevel;
    result = 0;
#endif
    return result;
}