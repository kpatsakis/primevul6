static int fsmLink(int odirfd, const char *opath, int dirfd, const char *path)
{
    int rc = linkat(odirfd, opath, dirfd, path, 0);

    if (_fsm_debug) {
	rpmlog(RPMLOG_DEBUG, " %8s (%d %s, %d %s) %s\n", __func__,
	       odirfd, opath, dirfd, path, (rc < 0 ? strerror(errno) : ""));
    }

    if (rc < 0)
	rc = RPMERR_LINK_FAILED;
    return rc;
}