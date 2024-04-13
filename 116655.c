static int fsmStat(int dirfd, const char *path, int dolstat, struct stat *sb)
{
    int flags = dolstat ? AT_SYMLINK_NOFOLLOW : 0;
    int rc = fstatat(dirfd, path, sb, flags);

    if (_fsm_debug && rc && errno != ENOENT)
        rpmlog(RPMLOG_DEBUG, " %8s (%d %s, ost) %s\n",
               __func__,
               dirfd, path, (rc < 0 ? strerror(errno) : ""));
    if (rc < 0) {
        rc = (errno == ENOENT ? RPMERR_ENOENT : RPMERR_LSTAT_FAILED);
	/* Ensure consistent struct content on failure */
        memset(sb, 0, sizeof(*sb));
    }
    return rc;
}