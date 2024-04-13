int rpmPackageFilesRemove(rpmts ts, rpmte te, rpmfiles files,
              rpmpsm psm, char ** failedFile)
{
    struct diriter_s di = { -1 };
    rpmfi fi = fsmIter(NULL, files, RPMFI_ITER_BACK, &di);
    rpmfs fs = rpmteGetFileStates(te);
    rpmPlugins plugins = rpmtsPlugins(ts);
    int fc = rpmfilesFC(files);
    int fx = -1;
    struct filedata_s *fdata = xcalloc(fc, sizeof(*fdata));
    int rc = 0;

    while (!rc && (fx = rpmfiNext(fi)) >= 0) {
	struct filedata_s *fp = &fdata[fx];
	fp->action = rpmfsGetAction(fs, rpmfiFX(fi));

	if (XFA_SKIPPING(fp->action))
	    continue;

	fp->fpath = fsmFsPath(fi, NULL);
	/* If the directory doesn't exist there's nothing to clean up */
	if (ensureDir(NULL, rpmfiDN(fi), 0, 0, 1, &di.dirfd))
	    continue;

	rc = fsmStat(di.dirfd, fp->fpath, 1, &fp->sb);

	fsmDebug(fp->fpath, fp->action, &fp->sb);

	/* Run fsm file pre hook for all plugins */
	rc = rpmpluginsCallFsmFilePre(plugins, fi, fp->fpath,
				      fp->sb.st_mode, fp->action);

	rc = fsmBackup(di.dirfd, fi, fp->action);

        /* Remove erased files. */
        if (fp->action == FA_ERASE) {
	    int missingok = (rpmfiFFlags(fi) & (RPMFILE_MISSINGOK | RPMFILE_GHOST));

	    rc = fsmRemove(di.dirfd, fp->fpath, fp->sb.st_mode);

	    /*
	     * Missing %ghost or %missingok entries are not errors.
	     * XXX: Are non-existent files ever an actual error here? Afterall
	     * that's exactly what we're trying to accomplish here,
	     * and complaining about job already done seems like kinderkarten
	     * level "But it was MY turn!" whining...
	     */
	    if (rc == RPMERR_ENOENT && missingok) {
		rc = 0;
	    }

	    /*
	     * Dont whine on non-empty directories for now. We might be able
	     * to track at least some of the expected failures though,
	     * such as when we knowingly left config file backups etc behind.
	     */
	    if (rc == RPMERR_ENOTEMPTY) {
		rc = 0;
	    }

	    if (rc) {
		int lvl = strict_erasures ? RPMLOG_ERR : RPMLOG_WARNING;
		rpmlog(lvl, _("%s %s: remove failed: %s\n"),
			S_ISDIR(fp->sb.st_mode) ? _("directory") : _("file"),
			fp->fpath, strerror(errno));
            }
        }

	/* Run fsm file post hook for all plugins */
	rpmpluginsCallFsmFilePost(plugins, fi, fp->fpath,
				  fp->sb.st_mode, fp->action, rc);

        /* XXX Failure to remove is not (yet) cause for failure. */
        if (!strict_erasures) rc = 0;

	if (rc)
	    *failedFile = xstrdup(fp->fpath);

	if (rc == 0) {
	    /* Notify on success. */
	    /* On erase we're iterating backwards, fixup for progress */
	    rpm_loff_t amount = rpmfiFC(fi) - rpmfiFX(fi);
	    rpmpsmNotify(psm, RPMCALLBACK_UNINST_PROGRESS, amount);
	}
    }

    for (int i = 0; i < fc; i++)
	free(fdata[i].fpath);
    free(fdata);
    fsmIterFini(fi, &di);

    return rc;
}