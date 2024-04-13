static int fsmSetFCaps(int fd, int dirfd, const char *path, const char *captxt)
{
    int rc = 0;

#if WITH_CAP
    if (captxt && *captxt != '\0') {
	cap_t fcaps = cap_from_text(captxt);

	if (fd >= 0) {
	    if (fcaps == NULL || cap_set_fd(fd, fcaps))
		rc = RPMERR_SETCAP_FAILED;
	} else {
	    if (fcaps == NULL || cap_set_fileat(dirfd, path, fcaps))
		rc = RPMERR_SETCAP_FAILED;
	}

	if (_fsm_debug) {
	    rpmlog(RPMLOG_DEBUG, " %8s (%d - %d %s, %s) %s\n", __func__,
		   fd, dirfd, path, captxt, (rc < 0 ? strerror(errno) : ""));
	}
	cap_free(fcaps);
    } 
#endif
    return rc;
}