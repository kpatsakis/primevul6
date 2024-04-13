msyslog(
	int		level,
	const char *	fmt,
	...
	)
{
	char	buf[1024];
	va_list	ap;

	va_start(ap, fmt);
	mvsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	addto_syslog(level, buf);
}