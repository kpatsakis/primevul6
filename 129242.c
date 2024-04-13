setup_logfile(
	const char *	name
	)
{
	if (NULL == syslog_fname && NULL != name) {
		if (-1 == change_logfile(name, TRUE))
			msyslog(LOG_ERR, "Cannot open log file %s, %m",
				name);
		return ;
	} 
	if (NULL == syslog_fname)
		return;

	if (-1 == change_logfile(syslog_fname, FALSE))
		msyslog(LOG_ERR, "Cannot reopen log file %s, %m",
			syslog_fname);
}