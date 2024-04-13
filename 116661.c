static int fsmReadLink(const char *path,
		       char *buf, size_t bufsize, size_t *linklen)
{
    ssize_t llen = readlink(path, buf, bufsize - 1);
    int rc = RPMERR_READLINK_FAILED;

    if (_fsm_debug) {
        rpmlog(RPMLOG_DEBUG, " %8s (%s, buf, %d) %s\n",
	       __func__,
               path, (int)(bufsize -1), (llen < 0 ? strerror(errno) : ""));
    }

    if (llen >= 0) {
	buf[llen] = '\0';
	rc = 0;
	*linklen = llen;
    }
    return rc;
}