static int fsmUtime(int fd, int dirfd, const char *path, mode_t mode, time_t mtime)
{
    int rc = 0;
    struct timespec stamps[2] = {
	{ .tv_sec = mtime, .tv_nsec = 0 },
	{ .tv_sec = mtime, .tv_nsec = 0 },
    };

    if (fd >= 0)
	rc = futimens(fd, stamps);
    else
	rc = utimensat(dirfd, path, stamps, AT_SYMLINK_NOFOLLOW);
    
    if (_fsm_debug)
	rpmlog(RPMLOG_DEBUG, " %8s (%d - %d %s, 0x%x) %s\n", __func__,
	       fd, dirfd, path, (unsigned)mtime, (rc < 0 ? strerror(errno) : ""));
    if (rc < 0)	rc = RPMERR_UTIME_FAILED;
    /* ...but utime error is not critical for directories */
    if (rc && S_ISDIR(mode))
	rc = 0;
    return rc;
}