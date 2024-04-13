change_logfile(
	const char *	fname,
	int		leave_crumbs
	)
{
	FILE *		new_file;
	const char *	log_fname;
	char *		abs_fname;
#if !defined(SYS_WINNT) && !defined(SYS_VXWORKS) && !defined(VMS)
	char		curdir[512];
	size_t		cd_octets;
	size_t		octets;
#endif	/* POSIX */

	REQUIRE(fname != NULL);
	log_fname = fname;

	/*
	 * In a forked child of a parent which is logging to a file
	 * instead of syslog, syslog_file will be NULL and both
	 * syslog_fname and syslog_abs_fname will be non-NULL.
	 * If we are given the same filename previously opened
	 * and it's still open, there's nothing to do here.
	 */
	if (syslog_file != NULL && syslog_fname != NULL &&
	    0 == strcmp(syslog_fname, log_fname))
		return 0;

	if (0 == strcmp(log_fname, "stderr")) {
		new_file = stderr;
		abs_fname = estrdup(log_fname);
	} else if (0 == strcmp(log_fname, "stdout")) {
		new_file = stdout;
		abs_fname = estrdup(log_fname);
	} else {
		if (syslog_fname != NULL &&
		    0 == strcmp(log_fname, syslog_fname))
			log_fname = syslog_abs_fname;
#if !defined(SYS_WINNT) && !defined(SYS_VXWORKS) && !defined(VMS)
		if (log_fname != syslog_abs_fname &&
		    DIR_SEP != log_fname[0] &&
		    0 != strcmp(log_fname, "stderr") &&
		    0 != strcmp(log_fname, "stdout") &&
		    NULL != getcwd(curdir, sizeof(curdir))) {
			cd_octets = strlen(curdir);
			/* trim any trailing '/' */
			if (cd_octets > 1 &&
			    DIR_SEP == curdir[cd_octets - 1])
				cd_octets--;
			octets = cd_octets;
			octets += 1;	/* separator '/' */
			octets += strlen(log_fname);
			octets += 1;	/* NUL terminator */
			abs_fname = emalloc(octets);
			snprintf(abs_fname, octets, "%.*s%c%s",
				 (int)cd_octets, curdir, DIR_SEP,
				 log_fname);
		} else
#endif
			abs_fname = estrdup(log_fname);
		TRACE(1, ("attempting to open log %s\n", abs_fname));
		new_file = fopen(abs_fname, "a");
	}

	if (NULL == new_file) {
		free(abs_fname);
		return -1;
	}

	/* leave a pointer in the old log */
	if (leave_crumbs && (syslogit || log_fname != syslog_abs_fname))
		msyslog(LOG_NOTICE, "switching logging to file %s",
			abs_fname);

	if (syslog_file != NULL &&
	    syslog_file != stderr && syslog_file != stdout &&
	    fileno(syslog_file) != fileno(new_file))
		fclose(syslog_file);
	syslog_file = new_file;
	if (log_fname == syslog_abs_fname) {
		free(abs_fname);
	} else {
		if (syslog_abs_fname != NULL &&
		    syslog_abs_fname != syslog_fname)
			free(syslog_abs_fname);
		if (syslog_fname != NULL)
			free(syslog_fname);
		syslog_fname = estrdup(log_fname);
		syslog_abs_fname = abs_fname;
	}
	syslogit = FALSE;

	return 0;
}