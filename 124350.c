println(const char* fmt, ...)
{
	char *tmp;
	int written;

	va_list ap;
	va_start (ap, fmt);
	written = vasprintf (&tmp, fmt, ap);

	dcprintf("%s%s", tmp, newlinestring);

	free(tmp);
}