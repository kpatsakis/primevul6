static int fsmOpen(int *wfdp, int dirfd, const char *dest)
{
    int rc = 0;
    /* Create the file with 0200 permissions (write by owner). */
    int fd = openat(dirfd, dest, O_WRONLY|O_EXCL|O_CREAT, 0200);

    if (fd < 0)
	rc = RPMERR_OPEN_FAILED;

    if (_fsm_debug) {
	rpmlog(RPMLOG_DEBUG, " %8s (%s [%d]) %s\n", __func__,
	       dest, fd, (rc < 0 ? strerror(errno) : ""));
    }
    *wfdp = fd;

    return rc;
}