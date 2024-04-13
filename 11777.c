check_pending(NCURSES_SP_DCL0)
/* check for pending input */
{
    bool have_pending = FALSE;

    /*
     * Only carry out this check when the flag is zero, otherwise we'll
     * have the refreshing slow down drastically (or stop) if there's an
     * unread character available.
     */
    if (SP_PARM->_fifohold != 0)
	return FALSE;

    if (SP_PARM->_checkfd >= 0) {
#if USE_FUNC_POLL
	struct pollfd fds[1];
	fds[0].fd = SP_PARM->_checkfd;
	fds[0].events = POLLIN;
	if (poll(fds, (size_t) 1, 0) > 0) {
	    have_pending = TRUE;
	}
#elif defined(__BEOS__)
	/*
	 * BeOS's select() is declared in socket.h, so the configure script does
	 * not see it.  That's just as well, since that function works only for
	 * sockets.  This (using snooze and ioctl) was distilled from Be's patch
	 * for ncurses which uses a separate thread to simulate select().
	 *
	 * FIXME: the return values from the ioctl aren't very clear if we get
	 * interrupted.
	 */
	int n = 0;
	int howmany = ioctl(0, 'ichr', &n);
	if (howmany >= 0 && n > 0) {
	    have_pending = TRUE;
	}
#elif HAVE_SELECT
	fd_set fdset;
	struct timeval ktimeout;

	ktimeout.tv_sec =
	    ktimeout.tv_usec = 0;

	FD_ZERO(&fdset);
	FD_SET(SP_PARM->_checkfd, &fdset);
	if (select(SP_PARM->_checkfd + 1, &fdset, NULL, NULL, &ktimeout) != 0) {
	    have_pending = TRUE;
	}
#endif
    }
    if (have_pending) {
	SP_PARM->_fifohold = 5;
	NCURSES_SP_NAME(_nc_flush) (NCURSES_SP_ARG);
    }
    return FALSE;
}