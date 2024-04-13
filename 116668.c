static int fsmMkfile(int dirfd, rpmfi fi, struct filedata_s *fp, rpmfiles files,
		     rpmpsm psm, int nodigest,
		     struct filedata_s ** firstlink, int *firstlinkfile,
		     int *fdp)
{
    int rc = 0;
    int fd = -1;

    if (*firstlink == NULL) {
	/* First encounter, open file for writing */
	rc = fsmOpen(&fd, dirfd, fp->fpath);
	/* If it's a part of a hardlinked set, the content may come later */
	if (fp->sb.st_nlink > 1) {
	    *firstlink = fp;
	    *firstlinkfile = fd;
	}
    } else {
	/* Create hard links for others and avoid redundant metadata setting */
	if (*firstlink != fp) {
	    rc = fsmLink(dirfd, (*firstlink)->fpath, dirfd, fp->fpath);
	}
	fd = *firstlinkfile;
    }

    /* If the file has content, unpack it */
    if (rpmfiArchiveHasContent(fi)) {
	if (!rc)
	    rc = fsmUnpack(fi, fd, psm, nodigest);
	/* Last file of hardlink set, ensure metadata gets set */
	if (*firstlink) {
	    fp->setmeta = 1;
	    *firstlink = NULL;
	    *firstlinkfile = -1;
	}
    }
    *fdp = fd;

    return rc;
}