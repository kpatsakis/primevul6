static int fsmUnlink(int dirfd, const char *path)
{
    int rc = 0;
    removeSBITS(path);
    rc = unlinkat(dirfd, path, 0);
    if (_fsm_debug)
	rpmlog(RPMLOG_DEBUG, " %8s (%d %s) %s\n", __func__,
	       dirfd, path, (rc < 0 ? strerror(errno) : ""));
    if (rc < 0)
	rc = (errno == ENOENT ? RPMERR_ENOENT : RPMERR_UNLINK_FAILED);
    return rc;
}