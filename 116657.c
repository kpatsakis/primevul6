static int fsmClose(int *wfdp)
{
    int rc = 0;
    if (wfdp && *wfdp >= 0) {
	int myerrno = errno;
	static int oneshot = 0;
	static int flush_io = 0;
	int fdno = *wfdp;

	if (!oneshot) {
	    flush_io = (rpmExpandNumeric("%{?_flush_io}") > 0);
	    oneshot = 1;
	}
	if (flush_io) {
	    fsync(fdno);
	}
	if (close(fdno))
	    rc = RPMERR_CLOSE_FAILED;

	if (_fsm_debug) {
	    rpmlog(RPMLOG_DEBUG, " %8s ([%d]) %s\n", __func__,
		   fdno, (rc < 0 ? strerror(errno) : ""));
	}
	*wfdp = -1;
	errno = myerrno;
    }
    return rc;
}