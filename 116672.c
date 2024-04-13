static int fsmChown(int fd, int dirfd, const char *path, mode_t mode, uid_t uid, gid_t gid)
{
    int rc;
    struct stat st;

    if (fd >= 0) {
	rc = fchown(fd, uid, gid);
	if (rc < 0) {
	    if (fstat(fd, &st) == 0 && (st.st_uid == uid && st.st_gid == gid)) {
		rc = 0;
	    }
	}
    } else {
	int flags = S_ISLNK(mode) ? AT_SYMLINK_NOFOLLOW : 0;
	rc = fchownat(dirfd, path, uid, gid, flags);
	if (rc < 0) {
	    struct stat st;
	    if (fstatat(dirfd, path, &st, flags) == 0 &&
		    (st.st_uid == uid && st.st_gid == gid)) {
		rc = 0;
	    }
	}
    }
    if (_fsm_debug) {
	rpmlog(RPMLOG_DEBUG, " %8s (%d - %d %s, %d, %d) %s\n", __func__,
	       fd, dirfd, path, (int)uid, (int)gid,
	       (rc < 0 ? strerror(errno) : ""));
    }
    if (rc < 0)	rc = RPMERR_CHOWN_FAILED;
    return rc;
}