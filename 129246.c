log_maybe(
	size_t     *pnerr,
	const char *fmt  ,
	...)
{
	va_list ap;
	if (++(*pnerr) <= nerr_loglimit) {
		va_start(ap, fmt);
		mvsyslog(LOG_ERR, fmt, ap);
		va_end(ap);
	}
}