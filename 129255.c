format_errmsg(
	char *		nfmt,
	size_t		lennfmt,
	const char *	fmt,
	int		errval
	)
{
	char errmsg[256];
	char c;
	char *n;
	const char *f;
	size_t len;

	n = nfmt;
	f = fmt;
	while ((c = *f++) != '\0' && n < (nfmt + lennfmt - 1)) {
		if (c != '%') {
			*n++ = c;
			continue;
		}
		if ((c = *f++) != 'm') {
			*n++ = '%';
			if ('\0' == c)
				break;
			*n++ = c;
			continue;
		}
		errno_to_str(errval, errmsg, sizeof(errmsg));
		len = strlen(errmsg);

		/* Make sure we have enough space for the error message */
		if ((n + len) < (nfmt + lennfmt - 1)) {
			memcpy(n, errmsg, len);
			n += len;
		}
	}
	*n = '\0';
}