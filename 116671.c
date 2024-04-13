static int fsmChmod(int fd, int dirfd, const char *path, mode_t mode)
{
    mode_t fmode = (mode & 07777);
    int rc;
    if (fd >= 0) {
	rc = fchmod(fd, fmode);
	if (rc < 0) {
	    struct stat st;
	    if (fstat(fd, &st) == 0 && (st.st_mode & 07777) == fmode) {
		rc = 0;
	    }
	}
    } else {
	rc = fchmodat(dirfd, path, fmode, 0);
	if (rc < 0) {
	    struct stat st;
	    if (fstatat(dirfd, path, &st, AT_SYMLINK_NOFOLLOW) == 0 &&
		    (st.st_mode & 07777) == fmode) {
		rc = 0;
	    }
	}
    }
    if (_fsm_debug)
	rpmlog(RPMLOG_DEBUG, " %8s (%d - %d %s, 0%04o) %s\n", __func__,
	       fd, dirfd, path, (unsigned)(mode & 07777),
	       (rc < 0 ? strerror(errno) : ""));
    if (rc < 0)	rc = RPMERR_CHMOD_FAILED;
    return rc;
}