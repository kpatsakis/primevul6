static int fsmUnpack(rpmfi fi, int fdno, rpmpsm psm, int nodigest)
{
    FD_t fd = fdDup(fdno);
    int rc = rpmfiArchiveReadToFilePsm(fi, fd, nodigest, psm);
    if (_fsm_debug) {
	rpmlog(RPMLOG_DEBUG, " %8s (%s %" PRIu64 " bytes [%d]) %s\n", __func__,
	       rpmfiFN(fi), rpmfiFSize(fi), Fileno(fd),
	       (rc < 0 ? strerror(errno) : ""));
    }
    Fclose(fd);
    return rc;
}