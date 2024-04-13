static int ensureDir(rpmPlugins plugins, const char *p, int owned, int create,
		    int quiet, int *dirfdp)
{
    char *path = xstrdup(p);
    char *dp = path;
    char *sp = NULL, *bn;
    int oflags = O_RDONLY;
    int rc = 0;

    if (*dirfdp >= 0)
	return rc;

    int dirfd = fsmOpenat(-1, "/", oflags);
    int fd = dirfd; /* special case of "/" */

    while ((bn = strtok_r(dp, "/", &sp)) != NULL) {
	struct stat sb;
	fd = fsmOpenat(dirfd, bn, oflags);

	if (fd < 0 && errno == ENOENT && create) {
	    mode_t mode = S_IFDIR | (_dirPerms & 07777);
	    rc = fsmDoMkDir(plugins, dirfd, bn, owned, mode);
	    if (!rc)
		fd = fsmOpenat(dirfd, bn, oflags|O_NOFOLLOW);
	}

	if (fd >= 0 && fstat(fd, &sb) == 0 && !S_ISDIR(sb.st_mode)) {
	    rc = RPMERR_ENOTDIR;
	    break;
	}

	close(dirfd);
	if (fd >= 0) {
	    dirfd = fd;
	} else {
	    if (!quiet) {
		rpmlog(RPMLOG_ERR, _("failed to open dir %s of %s: %s\n"),
			bn, p, strerror(errno));
	    }
	    rc = RPMERR_OPEN_FAILED;
	    break;
	}

	dp = NULL;
    }

    if (rc) {
	close(fd);
	close(dirfd);
	dirfd = -1;
    } else {
	rc = 0;
    }
    *dirfdp = dirfd;

    free(path);
    return rc;
}