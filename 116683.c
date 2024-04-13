static int fsmRmdir(int dirfd, const char *path)
{
    int rc = unlinkat(dirfd, path, AT_REMOVEDIR);
    if (_fsm_debug)
	rpmlog(RPMLOG_DEBUG, " %8s (%d %s) %s\n", __func__,
	       dirfd, path, (rc < 0 ? strerror(errno) : ""));
    if (rc < 0)
	switch (errno) {
	case ENOENT:        rc = RPMERR_ENOENT;    break;
	case ENOTEMPTY:     rc = RPMERR_ENOTEMPTY; break;
	default:            rc = RPMERR_RMDIR_FAILED; break;
	}
    return rc;
}