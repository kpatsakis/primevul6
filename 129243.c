mfprintf(
	FILE *		fp,
	const char *	fmt,
	...
	)
{
	va_list		ap;
	int		rc;

	va_start(ap, fmt);
	rc = mvfprintf(fp, fmt, ap);
	va_end(ap);

	return rc;
}