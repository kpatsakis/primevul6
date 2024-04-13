static int fsmMknod(int dirfd, const char *path, mode_t mode, dev_t dev)
{
    /* FIX: check S_IFIFO or dev != 0 */
    int rc = mknodat(dirfd, path, (mode & ~07777), dev);

    if (_fsm_debug) {
	rpmlog(RPMLOG_DEBUG, " %8s (%d %s, 0%o, 0x%x) %s\n",
	       __func__, dirfd, path, (unsigned)(mode & ~07777),
	       (unsigned)dev, (rc < 0 ? strerror(errno) : ""));
    }

    if (rc < 0)
	rc = RPMERR_MKNOD_FAILED;

    return rc;
}