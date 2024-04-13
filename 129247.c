msnprintf(
	char *		buf,
	size_t		bufsiz,
	const char *	fmt,
	...
	)
{
	va_list	ap;
	size_t	rc;

	va_start(ap, fmt);
	rc = mvsnprintf(buf, bufsiz, fmt, ap);
	va_end(ap);

	return rc;
}