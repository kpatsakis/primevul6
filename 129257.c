mprintf(
	const char *	fmt,
	...
	)
{
	va_list		ap;
	int		rc;

	va_start(ap, fmt);
	rc = mvfprintf(stdout, fmt, ap);
	va_end(ap);

	return rc;
}