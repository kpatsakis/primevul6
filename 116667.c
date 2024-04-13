static int fsmSymlink(const char *opath, int dirfd, const char *path)
{
    int rc = symlinkat(opath, dirfd, path);

    if (_fsm_debug) {
	rpmlog(RPMLOG_DEBUG, " %8s (%s, %d %s) %s\n", __func__,
	       opath, dirfd, path, (rc < 0 ? strerror(errno) : ""));
    }

    if (rc < 0)
	rc = RPMERR_SYMLINK_FAILED;
    return rc;
}