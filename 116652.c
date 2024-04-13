static int fsmRename(int odirfd, const char *opath, int dirfd, const char *path)
{
    removeSBITS(path);
    int rc = renameat(odirfd, opath, dirfd, path);
#if defined(ETXTBSY) && defined(__HPUX__)
    /* XXX HP-UX (and other os'es) don't permit rename to busy files. */
    if (rc && errno == ETXTBSY) {
	char *rmpath = NULL;
	rstrscat(&rmpath, path, "-RPMDELETE", NULL);
	/* Rename within the original directory */
	rc = renameat(odirfd, path, odirfd, rmpath);
	if (!rc) rc = renameat(odirfd, opath, dirfd, path);
	free(rmpath);
    }
#endif
    if (_fsm_debug)
	rpmlog(RPMLOG_DEBUG, " %8s (%d %s, %d %s) %s\n", __func__,
	       odirfd, opath, dirfd, path, (rc < 0 ? strerror(errno) : ""));
    if (rc < 0)
	rc = (errno == EISDIR ? RPMERR_EXIST_AS_DIR : RPMERR_RENAME_FAILED);
    return rc;
}