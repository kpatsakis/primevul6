static int fsmMkdir(int dirfd, const char *path, mode_t mode)
{
    int rc = mkdirat(dirfd, path, (mode & 07777));
    if (_fsm_debug)
	rpmlog(RPMLOG_DEBUG, " %8s (%d %s, 0%04o) %s\n", __func__,
	       dirfd, path, (unsigned)(mode & 07777),
	       (rc < 0 ? strerror(errno) : ""));
    if (rc < 0)	rc = RPMERR_MKDIR_FAILED;
    return rc;
}