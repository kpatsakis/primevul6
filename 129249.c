mvsyslog(
	int		level,
	const char *	fmt,
	va_list		ap
	)
{
	char	buf[1024];
	mvsnprintf(buf, sizeof(buf), fmt, ap);
	addto_syslog(level, buf);
}