static int fsmCommit(int dirfd, char **path, rpmfi fi, rpmFileAction action, const char *suffix)
{
    int rc = 0;

    /* XXX Special case /dev/log, which shouldn't be packaged anyways */
    if (!(S_ISSOCK(rpmfiFMode(fi)) && IS_DEV_LOG(*path))) {
	const char *nsuffix = (action == FA_ALTNAME) ? SUFFIX_RPMNEW : NULL;
	char *dest = *path;
	/* Construct final destination path (nsuffix is usually NULL) */
	if (suffix)
	    dest = fsmFsPath(fi, nsuffix);

	/* Rename temporary to final file name if needed. */
	if (dest != *path) {
	    rc = fsmRename(dirfd, *path, dirfd, dest);
	    if (!rc) {
		if (nsuffix) {
		    char * opath = fsmFsPath(fi, NULL);
		    rpmlog(RPMLOG_WARNING, _("%s created as %s\n"),
			   opath, dest);
		    free(opath);
		}
		free(*path);
		*path = dest;
	    }
	}
    }

    return rc;
}